// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CaptureTheFlagGameState.h"
#include "GameFramework/GameModeBase.h"
#include "CaptureTheFlagGameMode.generated.h"

enum class EPlayerTeam : uint8;

USTRUCT(BlueprintType)
struct FTeamPlayerData
{
	GENERATED_BODY()

	int NumPlayers;
	UPROPERTY()
	APlayerStart* Start;
};

UCLASS(minimalapi)
class ACaptureTheFlagGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CTF: Players")
	TMap<EPlayerTeam, FTeamPlayerData> TeamsMap;

	/*
	 * Should be even so teams are correctly balanced.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF: Players")
	int MaxPlayers = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF: Rules")
	int ScoreToWin = 3;

private:
	bool bIsPlayerStartCached;

public:
	ACaptureTheFlagGameMode();

	void IncrementScoreForTeam(EPlayerTeam Team);
	bool CheckWinConditionForTeam(EPlayerTeam ScoringTeam, int Score) const;
	void ResetGame();

private:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void SetupNewPlayer(APlayerController* NewPlayer, EPlayerTeam Team);
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingPlayer) override;

	static void SetPlayerLocationAt(AController* Player, const APlayerStart* PlayerStart);
};
