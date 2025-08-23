// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CaptureTheFlagPlayerController.generated.h"

class UHUDWidget;

/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY()
	UHUDWidget* HUDWidget;

protected:
	virtual void BeginPlay() override;
};
