// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CaptureTheFlagCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float Health)

/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API UCaptureTheFlagCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	FOnHealthChanged OnHealthChanged;
	
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Health, meta=(HideFromModifiers))
	FGameplayAttributeData Health = 100.0f;
	ATTRIBUTE_ACCESSORS(UCaptureTheFlagCharacterAttributeSet, Health)

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth = 100.0f;
	ATTRIBUTE_ACCESSORS(UCaptureTheFlagCharacterAttributeSet, MaxHealth)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UCaptureTheFlagCharacterAttributeSet, Damage)

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

private:
	UFUNCTION()
	void OnRep_Health() const;
	UFUNCTION()
	void OnRep_MaxHealth() const;
};
