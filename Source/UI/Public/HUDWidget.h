// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthWidget.h"
#include "ScoreWidget.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

enum class EPlayerTeam : uint8;
class UScoreWidget;

/**
 * 
 */
UCLASS()
class UI_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	UScoreWidget* ScoreWidget;

	UPROPERTY(meta=(BindWidget))
	UHealthWidget* HealthWidget;

public:
	UFUNCTION(BlueprintCallable)
	void SetScores(const int NewBlueTeamScore, const int NewRedTeamScore) const { ScoreWidget->SetScores(NewBlueTeamScore, NewRedTeamScore); }

	UFUNCTION(BlueprintCallable)
	void SetHealthBarPercentage(const float Percentage) const { HealthWidget->SetPercentage(Percentage); }
	UFUNCTION(BlueprintCallable)
	void SetHealthBarFillColor(const FLinearColor Color) const { HealthWidget->SetFillColor(Color); }
	UFUNCTION(BlueprintCallable)
	void SetHealthBarBorderColor(const FLinearColor Color) const { HealthWidget->SetBorderColor(Color); }
};
