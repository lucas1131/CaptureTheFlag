// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlagPlayerController.h"

#include "AbilitySystemComponent.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagGameState.h"
#include "CaptureTheFlagPlayerState.h"
#include "CountdownWidget.h"
#include "HUDWidget.h"
#include "MatchEndWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

void ACaptureTheFlagPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// No Setup for remote clients
	if (!IsLocalController()) return;

	const ACaptureTheFlagCharacter* PlayerCharacter = Cast<ACaptureTheFlagCharacter>(GetCharacter());

	if (IsValid(HUDClass))
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDClass, FName("HUD"));
		HUDWidget->SetScores(0, 0);
		HUDWidget->AddToViewport();
	}

	if (IsValid(MatchEndWidgetClass))
	{
		MatchEndWidget = CreateWidget<UMatchEndWidget>(this, MatchEndWidgetClass, FName("MatchEndWidget"));
		MatchEndWidget->AddToViewport();
		SetupMatchEndWidget();
		MatchEndWidget->OnBannerAnimationFinished.BindUObject(this, &ACaptureTheFlagPlayerController::StartMatchEndCountdown);
	}

	if (IsValid(RespawnCountdownWidgetClass))
	{
		if (PlayerCharacter)
		{
			RespawnCountdownWidget = CreateWidget<UCountdownWidget>(this, RespawnCountdownWidgetClass, FName("RespawnWidget"));
			RespawnCountdownWidget->SetVisibility(ESlateVisibility::Hidden);
			RespawnCountdownWidget->SetupAbilityComponent(PlayerCharacter->GetAbilitySystemComponent());
			RespawnCountdownWidget->AddToViewport();

			// Death triggered (respawn timer started)
			PlayerCharacter
				->GetAbilitySystemComponent()
				->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &ACaptureTheFlagPlayerController::StartRespawnCountdown);

			// Respawn triggered (tag removed)
			PlayerCharacter
				->GetAbilitySystemComponent()
				->RegisterGameplayTagEvent(RespawnCooldownTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &ACaptureTheFlagPlayerController::OnRespawned);
		}
	}

	if (ACaptureTheFlagGameState* State = GetWorld()->GetGameState<ACaptureTheFlagGameState>())
	{
		State->OnScoreChanged.AddUObject(this, &ACaptureTheFlagPlayerController::OnScoreChanged);
		State->OnMatchEnded.AddUObject(this, &ACaptureTheFlagPlayerController::OnMatchEnded);
		State->OnMatchReset.AddUObject(this, &ACaptureTheFlagPlayerController::OnMatchReset);
		MatchRestartTime = State->GetMatchRestartTime();
	}

	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetOnHealthChangedEvent().AddUObject(this, &ACaptureTheFlagPlayerController::OnHealthChanged);
	}
}

void ACaptureTheFlagPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	HideHealthForSpectator();
}

void ACaptureTheFlagPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACaptureTheFlagPlayerController::StartRespawnCountdown(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Effect, FActiveGameplayEffectHandle Handle) const
{
	if (!IsValid(RespawnCountdownWidget) || !Effect.Def->GetGrantedTags().HasTag(RespawnCooldownTag)) return;

	if (RespawnTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	}

	RespawnCountdownWidget->SetVisibility(ESlateVisibility::Visible);

	float Duration = ASC->GetGameplayEffectDuration(Handle);
	RespawnCooldownElapsedTime = 0;
	GetWorld()
		->GetTimerManager()
		.SetTimer(RespawnTimerHandle, FTimerDelegate::CreateLambda([this, Duration]() { CountdownRespawnTime(Duration); }), 0.1f, true);
}

void ACaptureTheFlagPlayerController::CountdownRespawnTime(float Duration) const
{
	// Here I add 0.9 to make the cooldown visuals more natural. Instead of popping a 3 for one frame then counting from 2 > 1 > 0,
	// This makes the countdown go to 3 > 2 > 1, which I believe feels more natural for players. Most people when counting down will go
	// until 1 and stop, its just the programmer mindset that tends to go to 0.
	const float RemainingTime = FMath::Max(Duration - RespawnCooldownElapsedTime, 1.0f) + 0.9f;
	RespawnCountdownWidget->SetCooldown(RemainingTime);
	if (RespawnCooldownElapsedTime >= Duration)
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	}
	RespawnCooldownElapsedTime += 0.1f;
}

void ACaptureTheFlagPlayerController::PlayEffects(UCameraComponent* Camera)
{
	PlayerCameraManager->StartCameraShake(CameraShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal);

	// This should probably be on a  custom camera component for the game
	// As I know my vignette is the only effect here, just going to access it directly.
	Camera->PostProcessSettings.WeightedBlendables.Array[0].Weight = 1.0f;

	if (VignetteEffectHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VignetteEffectHandle);
	}

	const auto InterpolateVignetteCallback = FTimerDelegate::CreateLambda([this, Camera]()
	{
		float Weight = Camera->PostProcessSettings.WeightedBlendables.Array[0].Weight;
		VignetteInterpolationSpeed = 1.0f;
		Weight = FMath::FInterpTo(Weight, 0.0f, 0.01f, VignetteInterpolationSpeed);

		if (Weight <= 0.01)
		{
			Weight = 0.0f;
			GetWorld()->GetTimerManager().ClearTimer(VignetteEffectHandle);
		}
		Camera->PostProcessSettings.WeightedBlendables.Array[0].Weight = Weight;
	});

	GetWorld()
		->GetTimerManager()
		.SetTimer(VignetteEffectHandle, InterpolateVignetteCallback, 0.01f, true);
}

void ACaptureTheFlagPlayerController::SetHealthBarPercentage(const float Percentage) const
{
	HUDWidget->SetHealthBarPercentage(Percentage);
}

void ACaptureTheFlagPlayerController::OnTookDamage_Implementation(float OldHealth, float NewHealth)
{
	const ACaptureTheFlagCharacter* CTFCharacter = Cast<ACaptureTheFlagCharacter>(GetCharacter());
	if (IsValid(CTFCharacter))
	{
		if (IsValid(HUDWidget))
		{
			SetHealthBarPercentage(NewHealth / CTFCharacter->GetMaxHealth());
		}

		UCameraComponent* Camera = CTFCharacter->GetPlayerCamera();
		if (IsLocalController() && Camera)
		{
			PlayEffects(Camera);
		}
	}
}

void ACaptureTheFlagPlayerController::SetupMatchEndWidget() const
{
	MatchEndWidget->SetVisibility(ESlateVisibility::Hidden);
	MatchEndWidget->SetRestartVisibility(ESlateVisibility::Hidden);
	MatchEndWidget->SetRestartCountdown(MatchRestartTime);
}

void ACaptureTheFlagPlayerController::ResetMatchEndRestartCountdown() const
{
	if (IsValid(MatchEndWidget))
	{
		MatchEndWidget->SetRestartCountdown(MatchRestartTime);
	}
}

void ACaptureTheFlagPlayerController::HideHealthForSpectator() const
{
	const ACaptureTheFlagPlayerState* CTFPlayerState = GetPlayerState<ACaptureTheFlagPlayerState>();
	if (IsValid(HUDWidget) && IsValid(CTFPlayerState) && CTFPlayerState->GetTeam() == EPlayerTeam::Spectator)
	{
		HUDWidget->SetHealthBarVisibility(ESlateVisibility::Hidden);
	}
}

void ACaptureTheFlagPlayerController::StartMatchEndCountdown()
{
	if (!IsValid(MatchEndWidget)) return;
	
	MatchEndWidget->SetRestartVisibility(ESlateVisibility::Visible);
	CurrentCountDown = MatchRestartTime;

	GetWorld()
		->GetTimerManager()
		.SetTimer(CountdownHandle,
		          FTimerDelegate::CreateUObject(this, &ACaptureTheFlagPlayerController::MatchResetTimerCountdown),
		          1,
		          true);
}

void ACaptureTheFlagPlayerController::MatchResetTimerCountdown()
{
	CurrentCountDown = FMath::Max(0, CurrentCountDown - 1);
	const int TruncatedCountdown = (int)CurrentCountDown;
	if (TruncatedCountdown <= 0)
	{
		if (IsValid(MatchEndWidget))
		{
			MatchEndWidget->SetRestartCountdown(TruncatedCountdown);
		}
		GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	}
}

void ACaptureTheFlagPlayerController::OnScoreChanged(const int BlueTeamScore, const int RedTeamScore) const
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->SetScores(BlueTeamScore, RedTeamScore);
	}
}

void ACaptureTheFlagPlayerController::OnMatchEnded(const EPlayerTeam WinnerTeam, const FLinearColor WinnerColor) const
{
	if (IsValid(MatchEndWidget))
	{
		MatchEndWidget->SetVisibility(ESlateVisibility::Visible);
		const FString TeamName = WinnerTeam == EPlayerTeam::Red ? TEXT("Red") : TEXT("Blue");
		MatchEndWidget->SetupAndPlayBannerAnimation(TeamName, WinnerColor);
	}
}

void ACaptureTheFlagPlayerController::OnMatchReset() const
{
	if (IsValid(MatchEndWidget))
	{
		SetupMatchEndWidget();
	}
}

void ACaptureTheFlagPlayerController::OnRespawned(FGameplayTag GameplayTag, const int NumTags) const
{
	if (IsValid(RespawnCountdownWidget) && NumTags == 0) // Want to trigger on tag removed
	{
		RespawnCountdownWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ACaptureTheFlagPlayerController::OnHealthChanged(const float OldHealth, const float NewHealth)
{
	// Took damage
	if (OldHealth > NewHealth)
	{
		OnTookDamage(OldHealth, NewHealth);
	}
	else // Healed or respawned
	{
		const ACaptureTheFlagCharacter* CTFCharacter = Cast<ACaptureTheFlagCharacter>(GetCharacter());
		if (IsValid(HUDWidget) && IsValid(CTFCharacter))
		{
			SetHealthBarPercentage(NewHealth / CTFCharacter->GetMaxHealth());
		}
	}
}
