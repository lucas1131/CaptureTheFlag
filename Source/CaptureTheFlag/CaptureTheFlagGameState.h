#pragma once
#include "CaptureTheFlagFlagActor.h"
#include "GameFramework/GameState.h"

#include "CaptureTheFlagGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnScoreChangedDelegate, int BlueTeamScore, int RedTeamScore);
DECLARE_DELEGATE_TwoParams(FOnMatchEnded, EPlayerTeam WinnerTeam, FLinearColor WinnerColor);
DECLARE_DELEGATE(FOnMatchReset);

UCLASS()
class ACaptureTheFlagGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	FOnScoreChangedDelegate OnScoreChanged;
	FOnMatchReset OnMatchReset;
	FOnMatchEnded OnMatchEnded;
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_UpdateScore)
	int BlueTeamScore;
	UPROPERTY(ReplicatedUsing=OnRep_UpdateScore)
	int RedTeamScore;

	UPROPERTY(EditDefaultsOnly, Category="CTF: Rules")
	float RestartTimeOnMatchEnd = 3;

public:
	ACaptureTheFlagGameState();
	void ResetScores();
	int IncrementScoreForTeam(EPlayerTeam Team);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	float GetMatchRestartTime() const { return RestartTimeOnMatchEnd; }
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMatchEnded(EPlayerTeam WinnerTeam, FLinearColor WinnerColor);
	void MulticastOnMatchEnded_Implementation(EPlayerTeam WinnerTeam, FLinearColor WinnerColor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMatchReset();
	void MulticastOnMatchReset_Implementation();

private:
	UFUNCTION()
	void OnRep_UpdateScore() const;
};
