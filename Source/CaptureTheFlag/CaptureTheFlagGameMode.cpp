// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"

#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagGameState.h"
#include "CaptureTheFlagPlayerController.h"
#include "CaptureTheFlagPlayerState.h"
#include "CaptureTheFlagProjectile.h"
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
	GameStateClass = ACaptureTheFlagGameState::StaticClass();
	PlayerControllerClass = ACaptureTheFlagPlayerController::StaticClass();

	TeamsMap.Add(EPlayerTeam::Spectator, FTeamPlayerData());
	TeamsMap.Add(EPlayerTeam::Blue, FTeamPlayerData());
	TeamsMap.Add(EPlayerTeam::Red, FTeamPlayerData());

	TeamColors.Add(EPlayerTeam::Spectator, FColor::White);
	TeamColors.Add(EPlayerTeam::Red, FColor(255, 20.4, 12.75));
	TeamColors.Add(EPlayerTeam::Blue, FColor(20.4, 50, 255));
}

void ACaptureTheFlagGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
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
			else if (PlayerStart->PlayerStartTag == FName("Spectator"))
			{
				TeamsMap[EPlayerTeam::Spectator].Start = PlayerStart;
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

void ACaptureTheFlagGameMode::SetupNewPlayer(APlayerController* NewPlayer, const EPlayerTeam Team)
{
	ACaptureTheFlagPlayerState* NewPlayerState = NewPlayer->GetPlayerState<ACaptureTheFlagPlayerState>();
	NewPlayerState->SetTeam(Team);
	TeamsMap[Team].NumPlayers++;

	// For now, player name is mostly for debugging since there is no input for player name
	const int NumPlayers = TeamsMap[EPlayerTeam::Blue].NumPlayers + TeamsMap[EPlayerTeam::Blue].NumPlayers;
	const FString PlayerName = NewPlayer->IsLocalController() ? TEXT("Host") : FString::Printf(TEXT("Client %d"), NumPlayers);
	NewPlayerState->SetPlayerName(PlayerName); 
	
	SetPlayerLocationAt(NewPlayer, TeamsMap[Team].Start);

	if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(NewPlayer->GetCharacter()))
	{
		Character->SetPlayerTint(TeamColors[Team]);
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

		// TODO Spectator isn't being positioned correctly yet but I consider this lower priority, leaving for later
		// Solution could be making a spectator pawn subclass that I can set a pending spawn location/rotation via PlayerState so it can position itself 
		SetupNewPlayer(NewPlayer, EPlayerTeam::Spectator);
		return;
	}

	const bool bBlueTeamHasLessPlayers = TeamsMap[EPlayerTeam::Blue].NumPlayers < TeamsMap[EPlayerTeam::Red].NumPlayers;
	SetupNewPlayer(NewPlayer, bBlueTeamHasLessPlayers ? EPlayerTeam::Blue : EPlayerTeam::Red);
}

void ACaptureTheFlagGameMode::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);
	const EPlayerTeam Team = ExitingPlayer->GetPlayerState<ACaptureTheFlagPlayerState>()->GetTeam();

	TArray<AActor*> Actors;
	ExitingPlayer->GetAttachedActors(Actors, true, true);
	for(AActor* Actor : Actors)
	{
		Actor->Destroy();
	}
	
	TeamsMap[Team].NumPlayers--;
}

void ACaptureTheFlagGameMode::SetPlayerLocationAt(AController* Player, const APlayerStart* PlayerStart)
{
	const FVector Location = PlayerStart->GetActorLocation();
	const FRotator Rotation = PlayerStart->GetActorRotation();
	Player->SetControlRotation(Rotation);
	if(APawn* Pawn = Player->GetPawn())
	{
		Pawn->SetActorLocationAndRotation(Location, Rotation);
	}
}

void ACaptureTheFlagGameMode::IncrementScoreForTeam(const EPlayerTeam Team)
{
	ACaptureTheFlagGameState* CTFGameState = GetGameState<ACaptureTheFlagGameState>();
	const int TeamScore = CTFGameState->IncrementScoreForTeam(Team);
	
	if (CheckWinConditionForTeam(Team, TeamScore))
	{
		EndGame(Team, CTFGameState); 
	}
}

bool ACaptureTheFlagGameMode::CheckWinConditionForTeam(const EPlayerTeam ScoringTeam, const int Score) const
{
	if (Score >= ScoreToWin)
	{
		ACaptureTheFlagGameState* CTFGameState = GetGameState<ACaptureTheFlagGameState>();
		
		for (const TObjectPtr<APlayerState> PlayerState : CTFGameState->PlayerArray)
		{
			if (ACaptureTheFlagPlayerState* CTFPlayerState = Cast<ACaptureTheFlagPlayerState>(PlayerState))
			{
				if (CTFPlayerState->GetTeam() == ScoringTeam)
				{
					CTFPlayerState->SetWinnerState(EMatchState::Win);
				}
				else if (CTFPlayerState->GetTeam() != EPlayerTeam::Spectator)
				{
					CTFPlayerState->SetWinnerState(EMatchState::Lose);
				}
				// TODO what to show to spectators? low priority for now
			}
		}

		return true;
	}

	return false;
}

void ACaptureTheFlagGameMode::DestroyAllActorsOfClass(UClass* ActorClass) const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, Actors);
	for (AActor* Actor : Actors)
	{
		Actor->Destroy();
	}
}

void ACaptureTheFlagGameMode::ResetGameState(ACaptureTheFlagGameState* CTFGameState)
{
	CTFGameState->ResetScores();
	
	for (const TObjectPtr<APlayerState> PlayerState : CTFGameState->PlayerArray)
	{
		if (const ACaptureTheFlagPlayerState* CTFPlayerState = Cast<ACaptureTheFlagPlayerState>(PlayerState))
		{
			const EPlayerTeam PlayerTeam = CTFPlayerState->GetTeam();
			PlayerState->GetPawn()->SetActorLocation(TeamsMap[PlayerTeam].Start->GetActorLocation());
		}
	}

	// TODO should also reset those physics cubes in scene but that's low priority for now
}

void ACaptureTheFlagGameMode::StartGame()
{
	ACaptureTheFlagGameState* CTFGameState = GetGameState<ACaptureTheFlagGameState>();
	ResetGameState(CTFGameState);
	DestroyAllActorsOfClass(ACaptureTheFlagProjectile::StaticClass());
	CTFGameState->MulticastOnMatchStarted();
}

void ACaptureTheFlagGameMode::EndGame(const EPlayerTeam Team, ACaptureTheFlagGameState* CTFGameState)
{
	CTFGameState->MulticastOnMatchEnded(Team, TeamColors[Team]);
		
	FTimerHandle ResetTimer;
	GetWorld()
		->GetTimerManager()
		.SetTimer(ResetTimer,
		          FTimerDelegate::CreateLambda([this](){ ResetGame(); }),
		          CTFGameState->GetMatchRestartTime()+1, // add a little extra time for visual animations, im not sure how to best handle this yet
		          false);
}

void ACaptureTheFlagGameMode::ResetGame()
{
	ACaptureTheFlagGameState* CTFGameState = GetGameState<ACaptureTheFlagGameState>();
	ResetGameState(CTFGameState);
	CTFGameState->MulticastOnMatchReset();
}


