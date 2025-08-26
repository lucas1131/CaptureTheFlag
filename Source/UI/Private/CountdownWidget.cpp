#include "CountdownWidget.h"

void UCountdownWidget::SetCooldown(float InTime)
{
	Time = InTime;
}

float UCountdownWidget::GetRemainingCountdown() const
{
	return Time;
}
