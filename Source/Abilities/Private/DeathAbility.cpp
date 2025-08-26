#include "DeathAbility.h"

#include "CaptureTheFlag/CaptureTheFlagCharacter.h"
#include "CaptureTheFlag/CaptureTheFlagPlayerController.h"

UDeathAbility::UDeathAbility(): DeathEventEffectClass(nullptr), DeathCooldownEffectClass(nullptr)
{
	FAbilityTriggerData Trigger;
	Trigger.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Event.Death"));
	Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(Trigger);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                    const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (ActorInfo->AbilitySystemComponent.IsValid() && IsValid(DeathEventEffectClass))
		{
			ActorInfo->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
				DeathEventEffectClass, 1, ActorInfo->AbilitySystemComponent->MakeEffectContext());
		}
		
		if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(ActorInfo->AvatarActor))
		{
			Character->SetRagdoll(true);
			Character->DisableInput(nullptr);
			Character->DropFlag();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UDeathAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	if (const ACaptureTheFlagPlayerController* PlayerController = Cast<ACaptureTheFlagPlayerController>(ActorInfo->PlayerController.Get()))
	{
		if (IsValid(DeathCooldownEffectClass))
		{
			const FActiveGameplayEffectHandle GEHandle = ActorInfo->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
				DeathCooldownEffectClass, 1, ActorInfo->AbilitySystemComponent->MakeEffectContext());
		}
	}
}
