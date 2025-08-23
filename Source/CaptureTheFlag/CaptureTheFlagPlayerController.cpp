// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlagPlayerController.h"

#include "CaptureTheFlagGameState.h"
#include "HUDWidget.h"
#include "MatchEndWidget.h"
#include "Blueprint/UserWidget.h"

void ACaptureTheFlagPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (IsValid(HUDClass))
		{
			HUDWidget = CreateWidget<UHUDWidget>(this, HUDClass, FName("HUD"));
			HUDWidget->AddToViewport();
			HUDWidget->SetScores(0, 0);
		}

		if (IsValid(MatchEndWidgetClass))
		{
			MatchEndWidget = CreateWidget<UMatchEndWidget>(this, MatchEndWidgetClass, FName("MatchEndWidget"));
			MatchEndWidget->AddToViewport();
			SetupMatchEndWidget();
			MatchEndWidget->OnBannerAnimationFinished.BindUObject(this, &ACaptureTheFlagPlayerController::StartCountdown);
		}
	}

	if (ACaptureTheFlagGameState* State = GetWorld()->GetGameState<ACaptureTheFlagGameState>())
	{
		State->OnScoreChanged.AddUObject(this, &ACaptureTheFlagPlayerController::OnScoreChanged);
		State->OnMatchEnded.BindUObject(this, &ACaptureTheFlagPlayerController::OnMatchEnded);
		State->OnMatchReset.BindUObject(this, &ACaptureTheFlagPlayerController::OnMatchReset);
		MatchRestartTime = State->GetMatchRestartTime();
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

void ACaptureTheFlagPlayerController::OnScoreChanged(const int BlueTeamScore, const int RedTeamScore)
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->SetScores(BlueTeamScore, RedTeamScore);
	}
}

void ACaptureTheFlagPlayerController::OnMatchEnded(const EPlayerTeam WinnerTeam, const FLinearColor WinnerColor)
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
