#include "CaptureTheFlagPlayerState.h"

#include "Net/UnrealNetwork.h"

void ACaptureTheFlagPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACaptureTheFlagPlayerState, Team);
	DOREPLIFETIME(ACaptureTheFlagPlayerState, WinState);
}

void ACaptureTheFlagPlayerState::Win()
{
	// TODO
}

void ACaptureTheFlagPlayerState::Lose()
{
	// TODO
}

void ACaptureTheFlagPlayerState::OnRep_SetWinState()
{
	if (WinState == EMatchState::Win)
	{
		Win();
	}
	else if (WinState == EMatchState::Lose)
	{
		Lose();
	}
}

