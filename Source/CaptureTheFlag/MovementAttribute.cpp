// Fill out your copyright notice in the Description page of Project Settings.
#include "MovementAttribute.h"

#include "Net/UnrealNetwork.h"

void UMovementAttribute::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMovementAttribute, SpeedMultiplier);
}
