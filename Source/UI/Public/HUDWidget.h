// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
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

public:
	UFUNCTION(BlueprintCallable)
	void SetScores(const int NewBlueTeamScore, const int NewRedTeamScore) const { ScoreWidget->SetScores(NewBlueTeamScore, NewRedTeamScore); }
};
