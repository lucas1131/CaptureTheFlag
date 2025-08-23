// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "BillboardWidgetComponent.generated.h"


UCLASS(MinimalAPI, meta=(BlueprintSpawnableComponent))
class UBillboardWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<APlayerController> LocalPlayerController;
	
public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
