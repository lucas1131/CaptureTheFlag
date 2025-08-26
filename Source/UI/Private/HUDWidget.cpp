// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

void UHUDWidget::ShowFlagIcon(const bool bShouldShow, const FLinearColor Tint) const
{
	FlagIcon->SetBrushTintColor(Tint);
	FlagIcon->SetVisibility(bShouldShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
