#include "CaptureTheFlagGameState.h"

#include "CaptureTheFlagPlayerState.h"
#include "Net/UnrealNetwork.h"

ACaptureTheFlagGameState::ACaptureTheFlagGameState(): BlueTeamScore(0), RedTeamScore(0){}

void ACaptureTheFlagGameState::ResetScores()
{
	BlueTeamScore = 0;
	RedTeamScore = 0;
	OnScoreChanged.Broadcast(BlueTeamScore, RedTeamScore);
}

int ACaptureTheFlagGameState::IncrementScoreForTeam(const EPlayerTeam Team)
{
	switch (Team)
	{
	case EPlayerTeam::Spectator:
		return 0;
	case EPlayerTeam::Blue:
		BlueTeamScore++;
		OnScoreChanged.Broadcast(BlueTeamScore, RedTeamScore);
		return BlueTeamScore;
	case EPlayerTeam::Red:
		RedTeamScore++;
		OnScoreChanged.Broadcast(BlueTeamScore, RedTeamScore);
		return RedTeamScore;
	}
	
	return 0;
}

void ACaptureTheFlagGameState::OnRep_UpdateScore() const
{
	OnScoreChanged.Broadcast(BlueTeamScore, RedTeamScore);
}

void ACaptureTheFlagGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureTheFlagGameState, RedTeamScore);
	DOREPLIFETIME(ACaptureTheFlagGameState, BlueTeamScore);
}
