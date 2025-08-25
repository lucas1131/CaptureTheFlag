// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagCharacterAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UCaptureTheFlagCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCaptureTheFlagCharacterAttributeSet, Health);
	DOREPLIFETIME(UCaptureTheFlagCharacterAttributeSet, MaxHealth);
}

void UCaptureTheFlagCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}

	Super::PreAttributeChange(Attribute, NewValue);
}

void UCaptureTheFlagCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast(NewValue);
	}
}

void UCaptureTheFlagCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float OldHealthValue = GetHealth();
		const float NewHealthValue = FMath::Clamp(OldHealthValue - GetDamage(), 0.0f, GetMaxHealth());

		if (OldHealthValue != NewHealthValue)
		{
			SetHealth(NewHealthValue);
		}

		SetDamage(0.0f);
	}
}

void UCaptureTheFlagCharacterAttributeSet::OnRep_Health() const
{
	OnHealthChanged.Broadcast(GetHealth());
}

void UCaptureTheFlagCharacterAttributeSet::OnRep_MaxHealth() const
{
	OnHealthChanged.Broadcast(GetHealth());
}
