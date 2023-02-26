// Copyright Epic Games, Inc. All Rights Reserved.

#include "GladiatorGameMode.h"
#include "GladiatorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGladiatorGameMode::AGladiatorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
