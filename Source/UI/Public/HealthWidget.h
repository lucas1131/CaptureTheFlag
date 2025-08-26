// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class UI_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category=HealthBar, meta=(BindWidget))
	UImage* HealthBarImage;
	UPROPERTY(EditAnywhere, Category=HealthBar)
	UMaterialInterface* HealthBarMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* HealthBarMID;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetPercentage(float Percentage) const;
	UFUNCTION(BlueprintCallable)
	void SetFillColor(FLinearColor Color) const;
	UFUNCTION(BlueprintCallable)
	void SetBorderColor(FLinearColor Color) const;
};
