// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"

#include "CaptureTheFlagPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode() : Super(), bIsPlayerStartCached(false)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerStateClass = ACaptureTheFlagPlayerState::StaticClass();

	TeamsMap.Add(EPlayerTeam::None, FTeamData()); // In case I decide later I want to use None as spectator
	TeamsMap.Add(EPlayerTeam::Blue, FTeamData());
	TeamsMap.Add(EPlayerTeam::Red, FTeamData());
}

void ACaptureTheFlagGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	TArray<AActor*> WorldActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), WorldActors);

	for (AActor* Actor : WorldActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(Actor);
		if (PlayerStart)
		{
			if (PlayerStart->PlayerStartTag == FName("Blue"))
			{
				TeamsMap[EPlayerTeam::Blue].Start = PlayerStart;
			}
			else if (PlayerStart->PlayerStartTag == FName("Red"))
			{
				TeamsMap[EPlayerTeam::Red].Start = PlayerStart;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Unknown player start tag '%s' found for object: '%s'"),
				       *PlayerStart->PlayerStartTag.ToString(),
				       *PlayerStart->GetName());
			}
		}
	}
}

void ACaptureTheFlagGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const int CurrentPlayerNum = TeamsMap[EPlayerTeam::Blue].NumPlayers + TeamsMap[EPlayerTeam::Red].NumPlayers;
	if (CurrentPlayerNum >= MaxPlayers)
	{
		APawn* Pawn = NewPlayer->GetPawn();
		NewPlayer->StartSpectatingOnly();
		Pawn->Destroy();
		return;
	}

	if (TeamsMap[EPlayerTeam::Blue].NumPlayers < TeamsMap[EPlayerTeam::Red].NumPlayers)
	{
		NewPlayer->GetPlayerState<ACaptureTheFlagPlayerState>()->SetTeam(EPlayerTeam::Blue);
		TeamsMap[EPlayerTeam::Blue].NumPlayers++;
		SetPlayerLocationAt(NewPlayer, TeamsMap[EPlayerTeam::Blue].Start);
	}
	else
	{
		NewPlayer->GetPlayerState<ACaptureTheFlagPlayerState>()->SetTeam(EPlayerTeam::Red);
		TeamsMap[EPlayerTeam::Red].NumPlayers++;
		SetPlayerLocationAt(NewPlayer, TeamsMap[EPlayerTeam::Red].Start);
	}
}

void ACaptureTheFlagGameMode::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);
	const EPlayerTeam Team = ExitingPlayer->GetPlayerState<ACaptureTheFlagPlayerState>()->GetTeam();
	TeamsMap[Team].NumPlayers--;
}

void ACaptureTheFlagGameMode::SetPlayerLocationAt(AController* Player, const APlayerStart* PlayerStart)
{
	const FVector Location = PlayerStart->GetActorLocation();
	const FRotator Rotation = PlayerStart->GetActorRotation();
	Player->SetControlRotation(Rotation);
	Player->GetPawn()->SetActorLocationAndRotation(Location, Rotation);
}

void ACaptureTheFlagGameMode::ResetGame()
{
	TeamsMap[EPlayerTeam::Blue].Score = 0;
	TeamsMap[EPlayerTeam::Red].Score = 0;

	for (const TObjectPtr<APlayerState> PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
	{
		if (const ACaptureTheFlagPlayerState* CTFPlayerState = Cast<ACaptureTheFlagPlayerState>(PlayerState))
		{
			const EPlayerTeam PlayerTeam = CTFPlayerState->GetTeam();
			PlayerState->GetPawn()->SetActorLocation(TeamsMap[PlayerTeam].Start->GetActorLocation());
		}
	}
}
