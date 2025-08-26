// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"

#include "Components/Image.h"

void UHealthWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Non-dynamic material for designer preview
	if (HealthBarImage && HealthBarMaterial)
	{
		HealthBarImage->SetBrushFromMaterial(HealthBarMaterial);
	}
}

void UHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (HealthBarImage && HealthBarMaterial)
	{
		HealthBarMID = UMaterialInstanceDynamic::Create(HealthBarMaterial, this);
		HealthBarImage->SetBrushFromMaterial(HealthBarMID);
	}
}

void UHealthWidget::SetPercentage(const float Percentage) const
{
	if(HealthBarMID)
	{
		HealthBarMID->SetScalarParameterValue("FillPercentage", Percentage);
	}
}

void UHealthWidget::SetFillColor(const FLinearColor Color) const
{
	if(HealthBarMID)
	{
		HealthBarMID->SetVectorParameterValue("FillColor", Color);
	}
}
void UHealthWidget::SetBorderColor(const FLinearColor Color) const
{
	if(HealthBarMID)
	{
		HealthBarMID->SetVectorParameterValue("BorderColor", Color);
	}
}
