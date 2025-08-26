// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
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
	FTimerHandle MatchRestartCountdownHandle;
	float MatchRestartTime;
	float CurrentCountDown;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UCountdownWidget> RespawnCountdownWidgetClass;
	FGameplayTag RespawnCooldownTag = FGameplayTag::RequestGameplayTag("Cooldown.Event.Death");
	UPROPERTY()
	UCountdownWidget* RespawnCountdownWidget;
	mutable FTimerHandle RespawnTimerHandle;
	mutable float RespawnCooldownElapsedTime;
	
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	float VignetteInterpolationSpeed;
	mutable FTimerHandle VignetteEffectHandle;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION()
	void StartRespawnCountdown(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Effect, FActiveGameplayEffectHandle Handle) const;
private:
	void CountdownRespawnTime(float Duration) const;

public:
	void PlayEffects(UCameraComponent* Camera) const;
	void SetHealthBarPercentage(float Percentage) const;
	UHUDWidget* GetHUDWidget() const { return HUDWidget; };

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnTookDamage(float OldHealth, float NewHealth);
	void OnTookDamage_Implementation(float OldHealth, float NewHealth);

private:
	void SetupMatchEndWidget() const;
	void ResetMatchEndRestartCountdown() const;
	void HideHealthForSpectator() const;

	UFUNCTION()
	void StartMatchEndCountdown();
	UFUNCTION()
	void MatchResetTimerCountdown();

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
