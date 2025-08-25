#include "CountdownWidget.h"

#include "AbilitySystemComponent.h"

float UCountdownWidget::GetRemainingCountdown() const
{
	// TODO need to see why this query is failing, gameplay debugger shows tag correctly
	const FGameplayTagContainer Tags(FGameplayTag::RequestGameplayTag("Cooldown.Event.Death"));
	TArray<FActiveGameplayEffectHandle> Effects = AbilitySystem->GetActiveEffectsWithAllTags(Tags);

	if (Effects.IsEmpty()) return 0;
	// Its expected to only have one of these
	const FActiveGameplayEffectHandle CountdownEffectHandle = Effects[0];
	if (const FActiveGameplayEffect* CountdownEffect = AbilitySystem->GetActiveGameplayEffect(CountdownEffectHandle))
	{
		return CountdownEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
	}

	return 0;
}
