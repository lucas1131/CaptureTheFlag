// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HealthAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float OldHealth, float NewHealth);

/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	FOnHealthChanged OnHealthChanged;
	
public:
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData CalculatedDamage;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, CalculatedDamage)
	
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health = 100.0f;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health)

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth = 100.0f;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
};
