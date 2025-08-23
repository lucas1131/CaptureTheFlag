// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreWidget.h"

#include "Components/TextBlock.h"

void UScoreWidget::SetScores(const int NewBlueTeamScore, const int NewRedTeamScore) const
{
	BlueTeamScore->SetText(FText::FromString(FString::FromInt(NewBlueTeamScore)));
	RedTeamScore->SetText(FText::FromString(FString::FromInt(NewRedTeamScore)));
}
