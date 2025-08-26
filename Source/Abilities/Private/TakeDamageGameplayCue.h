// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "UTakeDamageGameplayCue.generated.h"

/**
 * 
 */
UCLASS()
class ABILITIES_API UUTakeDamageGameplayCue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
};
