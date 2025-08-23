// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagPlayerController.h"

#include "CaptureTheFlagPlayerState.h"
#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"

void ACaptureTheFlagPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && IsValid(HUDClass))
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDClass, FName("HUD"));
		HUDWidget->AddToViewport();
	}

	// GetPlayerState<ACaptureTheFlagPlayerState>()
	
}
