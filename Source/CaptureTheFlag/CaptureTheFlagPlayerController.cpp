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
		MatchEndWidget->OnBannerAnimationFinished.BindUObject(this, &ACaptureTheFlagPlayerController::StartCountdown);
	}

	if (IsValid(RespawnCountdownWidgetClass))
	{
		if (PlayerCharacter)
		{
			RespawnCountdownWidget = CreateWidget<UCountdownWidget>(this, RespawnCountdownWidgetClass, FName("RespawnWidget"));
			RespawnCountdownWidget->SetVisibility(ESlateVisibility::Hidden);
			RespawnCountdownWidget->SetupAbilityComponent(PlayerCharacter->GetAbilitySystemComponent());
			RespawnCountdownWidget->AddToViewport();
			PlayerCharacter
				->GetAbilitySystemComponent()
				->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("Cooldown.Event.Death"), EGameplayTagEventType::NewOrRemoved)
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

void ACaptureTheFlagPlayerController::ShowRespawnCountdown(FGameplayTag GameplayTag, int _) const
{
	if (IsValid(RespawnCountdownWidget))
	{
		RespawnCountdownWidget->SetVisibility(ESlateVisibility::Visible);
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

void ACaptureTheFlagPlayerController::StartCountdown()
{
	if (IsValid(MatchEndWidget))
	{
		MatchEndWidget->SetRestartVisibility(ESlateVisibility::Visible);
		CurrentCountDown = MatchRestartTime;

		GetWorld()
			->GetTimerManager()
			.SetTimer(CountdownHandle,
			          FTimerDelegate::CreateUObject(this, &ACaptureTheFlagPlayerController::TimerCountdown),
			          1000, // TODO check if this is in seconds or ms
			          true);
	}
}

void ACaptureTheFlagPlayerController::TimerCountdown()
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
		OnTookDamage(OldHealth - NewHealth);
	}
}
