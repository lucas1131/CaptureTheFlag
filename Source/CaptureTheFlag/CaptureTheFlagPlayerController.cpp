// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlagPlayerController.h"

#include "CaptureTheFlagGameState.h"
#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"

void ACaptureTheFlagPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && IsValid(HUDClass))
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDClass, FName("HUD"));
		HUDWidget->AddToViewport();
		HUDWidget->SetScores(0, 0);
	}
	
	if (ACaptureTheFlagGameState* State = GetWorld()->GetGameState<ACaptureTheFlagGameState>())
	{
		State->OnScoreChanged.AddUObject(this, &ACaptureTheFlagPlayerController::OnScoreChanged);
	}
}

void ACaptureTheFlagPlayerController::OnScoreChanged(int BlueTeamScore, int RedTeamScore)
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->SetScores(BlueTeamScore, RedTeamScore);
	}
}