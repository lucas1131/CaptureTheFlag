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

UCLASS()
class ACaptureTheFlagPlayerState : public APlayerState
{
	GENERATED_BODY()
private:
	EPlayerTeam Team;

public:
	EPlayerTeam GetTeam() const { return Team; }
	void SetTeam(const EPlayerTeam InTeam) { Team = InTeam; }
};
