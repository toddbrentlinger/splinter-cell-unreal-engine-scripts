// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VaultingComponent.generated.h"

UENUM(BlueprintType)
enum class EVaultingState : uint8
{
	NotVaulting UMETA(DisplayName = "NotVaulting"),
	WantsToVault UMETA(DisplayName = "WantsToVault"),
	Vaulting UMETA(DisplayName = "Vaulting"),
};

UCLASS( ClassGroup=(Custom), BlueprintType, meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API UVaultingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVaultingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
