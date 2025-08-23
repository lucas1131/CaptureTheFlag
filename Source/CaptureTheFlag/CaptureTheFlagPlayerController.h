// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CaptureTheFlagGameState.h"
#include "GameFramework/PlayerController.h"
#include "CaptureTheFlagPlayerController.generated.h"

class UMatchEndWidget;
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
	TSubclassOf<UHUDWidget> HUDClass;

	UPROPERTY()
	UHUDWidget* HUDWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UMatchEndWidget> MatchEndWidgetClass;

	UPROPERTY()
	UMatchEndWidget* MatchEndWidget;
	FTimerHandle CountdownHandle;
	float MatchRestartTime;
	float CurrentCountDown;

protected:
	virtual void BeginPlay() override;

private:
	void SetupMatchEndWidget() const;
	void ResetMatchEndRestartCountdown() const;

	UFUNCTION()
	void StartCountdown();
	UFUNCTION()
	void TimerCountdown();

	UFUNCTION()
	void OnScoreChanged(int BlueTeamScore, int RedTeamScore);
	UFUNCTION()
	void OnMatchEnded(EPlayerTeam WinnerTeam, FLinearColor WinnerColor);
	UFUNCTION()
	void OnMatchReset() const;
};
