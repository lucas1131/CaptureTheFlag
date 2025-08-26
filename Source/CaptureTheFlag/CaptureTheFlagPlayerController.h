// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CaptureTheFlagPlayerController.generated.h"

struct FGameplayTag;
class UCountdownWidget;
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

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UCountdownWidget> RespawnCountdownWidgetClass;
	UPROPERTY()
	UCountdownWidget* RespawnCountdownWidget;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void ShowRespawnCountdown(FGameplayTag GameplayTag, int _) const;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnTookDamage(float OldHealth, float NewHealth);
	void OnTookDamage_Implementation(float OldHealth, float NewHealth);

private:
	void SetupMatchEndWidget() const;
	void ResetMatchEndRestartCountdown() const;

	UFUNCTION()
	void StartCountdown();
	UFUNCTION()
	void TimerCountdown();

	UFUNCTION()
	void OnScoreChanged(int BlueTeamScore, int RedTeamScore) const;
	UFUNCTION()
	void OnMatchEnded(EPlayerTeam WinnerTeam, FLinearColor WinnerColor) const;
	UFUNCTION()
	void OnMatchReset() const;
	UFUNCTION()
	void OnRespawned(FGameplayTag GameplayTag, int NumTags) const;
	UFUNCTION()
	void OnHealthChanged(float OldHealth, float NewHealth);
};
