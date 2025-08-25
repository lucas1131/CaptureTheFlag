// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthAttributeSet, Health);
	DOREPLIFETIME(UHealthAttributeSet, MaxHealth);
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
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

void UHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast(OldValue, NewValue);
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Only work on Calculated Damage, not base damage.
	if (Data.EvaluatedData.Attribute == GetCalculatedDamageAttribute())
	{
		const float OldHealthValue = GetHealth();
		const float Damage = GetCalculatedDamage();
		const float NewHealthValue = FMath::Clamp(OldHealthValue - Damage, 0.0f, GetMaxHealth());

		if (OldHealthValue != NewHealthValue)
		{
			SetHealth(NewHealthValue);
		}

		const AActor* AbilityActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		if (AbilityActor && AbilityActor->HasAuthority() && NewHealthValue <= UE_SMALL_NUMBER)
		{
			FGameplayEventData EventData;
			EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Death"));
			EventData.Instigator = Data.EffectSpec.GetContext().GetInstigator();
			EventData.Target = *Data.Target.AbilityActorInfo->AvatarActor;
			Data.Target.HandleGameplayEvent(EventData.EventTag, &EventData);
		}

		SetCalculatedDamage(0.0f);
	}
}

void UHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	OnHealthChanged.Broadcast(OldHealth.GetCurrentValue(), GetHealth());
}

void UHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	OnHealthChanged.Broadcast(OldMaxHealth.GetCurrentValue(), GetHealth());
}
