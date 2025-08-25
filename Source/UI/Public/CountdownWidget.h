#pragma once
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"

class UAbilitySystemComponent;

UCLASS()
class UI_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystem;
	
public:
	void SetupAbilityComponent(UAbilitySystemComponent* InASC) { AbilitySystem = InASC; }

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRemainingCountdown() const;
};
