// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractSourceBoxComponent.h"
#include "InteractInterface.h" // Replace InteractSourceBoxComponent with InteractInterface

#include "InteractManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct SPLINTERCELL_API FInteractSource
{
	GENERATED_BODY()

	UPROPERTY()
	UInteractSourceBoxComponent* InteractSourceComponent;

	UPROPERTY()
	uint32 bHasLineOfSight : 1;

	UPROPERTY()
	float Distance;

	FInteractSource()
	{
		InteractSourceComponent = nullptr;
	}

	FInteractSource(UInteractSourceBoxComponent* InteractSourceComponentToAdd)
	{
		InteractSourceComponent = InteractSourceComponentToAdd;
	}
};

UCLASS( ClassGroup = "Interact", meta = (BlueprintSpawnableComponent))
class SPLINTERCELL_API UInteractManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractManagerComponent();

	UPROPERTY(BlueprintReadWrite)
	TArray<FInteractSource> InteractSources;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void UpdateInteractSourcesToDisplay();

	UFUNCTION(BlueprintCallable)
	virtual bool DoesInteractSourceHaveLineOfSight(UInteractSourceBoxComponent* InteractSourceComponent);

	UFUNCTION(BlueprintCallable)
	virtual void SortInteractSources();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void AddInteractSource(UInteractSourceBoxComponent* InteractSourceComponent);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveInteractSource(UInteractSourceBoxComponent* InteractSourceComponent);
};
