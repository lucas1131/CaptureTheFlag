#pragma once
#include "Abilities/GameplayAbility.h"
#include "RespawnAbility.generated.h"

UCLASS(Blueprintable, ClassGroup=(Abilities, CaptureTheFlag))
class ABILITIES_API URespawnAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeathTagTimerEffect;
	
public:
	URespawnAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnRespawnCooldownFinished();

};

