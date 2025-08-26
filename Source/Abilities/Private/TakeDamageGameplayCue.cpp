// Fill out your copyright notice in the Description page of Project Settings.


#include "UTakeDamageGameplayCue.h"

#include "HUDWidget.h"
#include "CaptureTheFlag/CaptureTheFlagCharacter.h"
#include "CaptureTheFlag/CaptureTheFlagPlayerController.h"

bool UUTakeDamageGameplayCue::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	float CurrentHealth = Parameters.NormalizedMagnitude;

	if (const ACaptureTheFlagPlayerController* CTFPlayerController = Cast<ACaptureTheFlagPlayerController>(MyTarget->GetInstigatorController()))
	{
		if (UHUDWidget* HUDWidget = CTFPlayerController->GetHUDWidget())
		{
			if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(MyTarget))
			{
				HUDWidget->SetHealthBarPercentage(CurrentHealth / Character->GetMaxHealth());
				CTFPlayerController->PlayEffects(Character->GetPlayerCamera());
			}
		}
	}

	return true;
}
