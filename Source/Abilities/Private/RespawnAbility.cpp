#include "RespawnAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "CaptureTheFlag/CaptureTheFlagCharacter.h"
#include "CaptureTheFlag/CaptureTheFlagGameMode.h"

URespawnAbility::URespawnAbility()
{
	FAbilityTriggerData Trigger;
	Trigger.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Event.Death"));
	Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(Trigger);

	// SourceRequiredTags.AddTagFast(FGameplayTag::RequestGameplayTag(FName("Cooldown.Event.Death")));
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void URespawnAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	auto* WaitTask = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(
		this, FGameplayTag::RequestGameplayTag(FName("Cooldown.Event.Death")));

	WaitTask->Removed.AddDynamic(this, &URespawnAbility::OnRespawnCooldownFinished);
	WaitTask->ReadyForActivation();
}

void URespawnAbility::OnRespawnCooldownFinished()
{
    CurrentActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
    	DeathTagTimerEffect,
    	CurrentActorInfo->AbilitySystemComponent.Get(),
    	-1);

	if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(CurrentActorInfo->AvatarActor))
	{
		const APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get();
		ACaptureTheFlagGameMode* CTFGameMode = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());
		if (PlayerController && CTFGameMode)
		{
			CTFGameMode->MovePlayerBackToSpawn(PlayerController);
		}
		
		Character->InitializeCharacterAttributes();
		Character->EnableInput(nullptr);
		Character->SetRagdoll(false);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
