#pragma once
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"

UCLASS()
class UI_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	float Time;

public:
	void SetCooldown(float InTime);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRemainingCountdown() const;
};
