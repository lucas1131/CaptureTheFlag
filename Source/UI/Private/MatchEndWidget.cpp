// Fill out your copyright notice in the Description page of Project Settings.
#include "MatchEndWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

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
