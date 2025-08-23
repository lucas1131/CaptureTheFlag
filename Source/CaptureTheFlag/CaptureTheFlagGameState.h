#pragma once
#include "CaptureTheFlagFlagActor.h"
#include "GameFramework/GameState.h"

#include "CaptureTheFlagGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnScoreChangedDelegate, int BlueTeamScore, int RedTeamScore);

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

	FOnScoreChangedDelegate OnScoreChanged;

private:
	// UFUNCTION(NetMulticast, Reliable)
	// void MulticastOnScoreChanged(int NewBlueTeamScore, int NewRedTeamScore);
	// void MulticastOnScoreChanged_Implementation(int NewBlueTeamScore, int NewRedTeamScore);
	//
	UFUNCTION()
	void OnRep_UpdateScore() const;
};
