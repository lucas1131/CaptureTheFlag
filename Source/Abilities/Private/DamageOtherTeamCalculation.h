// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "CaptureTheFlag/HealthAttributeSet.h"
#include "DamageOtherTeamCalculation.generated.h"

struct FCapturedAttributes
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CalculatedDamage);


	FCapturedAttributes()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CalculatedDamage, Target, false);
	}
};

/**
 * 
 */
UCLASS()
class ABILITIES_API UDamageOtherTeamCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDamageOtherTeamCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
