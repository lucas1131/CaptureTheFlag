// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CaptureTheFlag/CaptureTheFlagPlayerState.h"
#include "ScoreWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UI_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* BlueTeamScore;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RedTeamScore;

public:
	UFUNCTION(BlueprintCallable)
	void SetScores(const int NewBlueTeamScore, const int NewRedTeamScore) const;
};
