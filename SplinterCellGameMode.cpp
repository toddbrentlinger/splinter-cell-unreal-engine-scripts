// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinterCellGameMode.h"
#include "SplinterCellCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASplinterCellGameMode::ASplinterCellGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bIsUsingAnalogStick = false;
}
