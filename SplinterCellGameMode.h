// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SplinterCellGameMode.generated.h"

UCLASS(minimalapi)
class ASplinterCellGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASplinterCellGameMode();

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	uint32 bIsUsingAnalogStick : 1;
};



