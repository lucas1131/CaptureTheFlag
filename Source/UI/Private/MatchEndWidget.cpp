// Fill out your copyright notice in the Description page of Project Settings.
#include "MatchEndWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UMatchEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FWidgetAnimationDynamicEvent OnAnimationStart;
	OnAnimationStart.BindDynamic(this, &UMatchEndWidget::OnBannerAnimationStarted);
	BindToAnimationFinished(SlideWinnerBannerAnim, OnAnimationStart);
	
	FWidgetAnimationDynamicEvent OnAnimationEnd;
	OnAnimationEnd.BindDynamic(this, &UMatchEndWidget::OnBannerAnimationEnded);
	BindToAnimationFinished(SlideWinnerBannerAnim, OnAnimationEnd);
}

void UMatchEndWidget::SetRestartVisibility(const ESlateVisibility NewVisibility) const
{
	CountdownCounterText->SetVisibility(NewVisibility);
}

void UMatchEndWidget::SetRestartCountdown(const int Seconds) const
{
	FFormatNamedArguments CountdownTextArgs;
	CountdownTextArgs.Add(TEXT("seconds"), FText::AsNumber(Seconds));
	const FText FormattedCountdown = FText::Format(CountdownTextFormat, CountdownTextArgs);
	CountdownCounterText->SetText(FormattedCountdown);
}

void UMatchEndWidget::SetupAndPlayBannerAnimation(const FString& WinnerTeamName, const FLinearColor& BannerColor)
{
	FFormatNamedArguments WinnerTextArgs;
	WinnerTextArgs.Add(TEXT("name"), FText::FromString(WinnerTeamName));
	const FText FormattedBannerText = FText::Format(BannerTextFormat, WinnerTextArgs);
	WinnerText->SetText(FormattedBannerText);
	WinnerTextShadow->SetText(FormattedBannerText);
	WinnerBanner->SetBrushColor(BannerColor);
	WinnerBannerShadow->SetBrushColor(BannerColor);

	PlayAnimation(SlideWinnerBannerAnim);
}

void UMatchEndWidget::OnBannerAnimationStarted()
{
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnBannerAnimationStartedDelegate.ExecuteIfBound();
}

void UMatchEndWidget::OnBannerAnimationEnded()
{
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnBannerAnimationFinishedDelegate.ExecuteIfBound();
}
