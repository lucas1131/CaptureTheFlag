#pragma once
#include "CaptureTheFlagFlagActor.h"
#include "GameFramework/GameState.h"

#include "CaptureTheFlagGameState.generated.h"


UCLASS()
class ACaptureTheFlagGameState : public AGameStateBase
{
	GENERATED_BODY()

private:
	UPROPERTY(ReplicatedUsing=OnRep_UpdateScore)
	int BlueTeamScore;
	UPROPERTY(ReplicatedUsing=OnRep_UpdateScore)
	int RedTeamScore;

public:
	ACaptureTheFlagGameState();
	void ResetScores();
	int IncrementScoreForTeam(EPlayerTeam Team);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_UpdateScore();
};
