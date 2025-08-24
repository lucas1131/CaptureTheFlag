// Fill out your copyright notice in the Description page of Project Settings.


#include "FireWeaponAbility.h"

#include "CaptureTheFlag/CaptureTheFlagCharacter.h"

UFireWeaponAbility::UFireWeaponAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Weapon.FireSemiAuto")));
}

bool UFireWeaponAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags,
                                            FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (const ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(ActorInfo->AvatarActor))
	{
		return Character->HasWeaponEquipped();
	}

	return false;
}

void UFireWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(ActorInfo->AvatarActor))
	{
		Character->FireWeapon();
	}

	// End immediately, no automatic fire for now
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
