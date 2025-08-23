#pragma once
#include "GameFramework/PlayerState.h"

#include "CaptureTheFlagPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeam : uint8
{
	Spectator = 0,
	Blue,
	Red,
};

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None = 0,
	Win,
	Lose
};

UCLASS()
class ACaptureTheFlagPlayerState : public APlayerState
{
	GENERATED_BODY()
private:
	UPROPERTY(Replicated)
	EPlayerTeam Team;
	
	UPROPERTY(ReplicatedUsing=OnRep_SetWinState)
	EMatchState WinState = EMatchState::None;

public:
	EPlayerTeam GetTeam() const { return Team; }
	void SetTeam(const EPlayerTeam InTeam) { Team = InTeam; }
	void SetWinnerState(const EMatchState InState) { WinState = InState; }

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Win();
	void Lose();
	UFUNCTION()
	void OnRep_SetWinState();
};
