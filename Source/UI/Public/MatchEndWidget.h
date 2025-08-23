// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchEndWidget.generated.h"

DECLARE_DELEGATE(FOnBannerAnimationStarted);
DECLARE_DELEGATE(FOnBannerAnimationFinished);

class UTextBlock;
class UBorder;

/**
 * 
 */
UCLASS()
class UI_API UMatchEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnBannerAnimationStarted OnBannerAnimationStarted;
	FOnBannerAnimationFinished OnBannerAnimationFinished;

private:
	UPROPERTY(EditAnywhere)
	FText CountdownTextFormat = FText::FromString(TEXT("Game restarts in {seconds}..."));
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CountdownCounterText;

	UPROPERTY(EditAnywhere)
	FText BannerTextFormat = FText::FromString(TEXT("{name} Team Wins!"));
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WinnerText;
	UPROPERTY(meta=(BindWidget))
	UBorder* WinnerBanner;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WinnerTextShadow;
	UPROPERTY(meta=(BindWidget))
	UBorder* WinnerBannerShadow;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* SlideWinnerBannerAnim;

public:
	UFUNCTION(BlueprintCallable)
	void SetRestartVisibility(const ESlateVisibility NewVisibility) const;
	UFUNCTION(BlueprintCallable)
	void SetRestartCountdown(const int Seconds) const;
	UFUNCTION(BlueprintCallable)
	void SetupAndPlayBannerAnimation(const FString& WinnerTeamName, const FLinearColor& BannerColor);
};
