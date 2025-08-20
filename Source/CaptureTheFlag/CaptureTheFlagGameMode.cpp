// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
