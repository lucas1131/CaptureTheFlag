#pragma once
#include "Abilities/GameplayAbility.h"
#include "DeathAbility.generated.h"

UCLASS(Blueprintable, ClassGroup=(Abilities, CaptureTheFlag))
class ABILITIES_API UDeathAbility : public UGameplayAbility
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeathEventEffectClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeathCooldownEffectClass;

public:
	UDeathAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
};
