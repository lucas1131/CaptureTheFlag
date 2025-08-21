// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class CAPTURETHEFLAG_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasRifle;

public:
	UFUNCTION(BlueprintCallable)
	void SetHasRifle(const bool bInHasRifle) { bHasRifle = bInHasRifle; };
};
