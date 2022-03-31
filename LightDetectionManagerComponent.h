// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightDetectionInterface.h"
#include "LightDetectionManagerComponent.generated.h"

class ULightDetector;
class ILightDetectionInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API ULightDetectionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Current light amount on Character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrLightAmount;

protected:

	/** Light detectors attacked to sockets on skeletal mesh. */
	UPROPERTY(BlueprintReadWrite)
	TArray<ULightDetector*> LightDetectors;

	/** Lights used to calculate the amount of light on the light detectors. Added when Character is inside trigger. */
	UPROPERTY(BlueprintReadWrite)
	TArray<UObject*> Lights;
	//TArray<TScriptInterface<ILightDetectionInterface>> Lights;

public:	
	// Sets default values for this component's properties
	ULightDetectionManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Returns total light amount on light detectors from lights in range. */
	UFUNCTION()
	virtual float CalculateTotalLightAmount();

	/** Assigns new value of current light amount. */
	UFUNCTION()
	virtual void SetCurrentLightAmount(float NewCurrLightAmount, float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Adds light to array of lights the Character is within range. */
	virtual void AddLight(UObject* NewLight);
	
	/** Removes light from array of lights the Character is within range. */
	virtual void RemoveLight(UObject* NewLight);

	/** Returns CurrLightAmount **/
	FORCEINLINE float GetCurrLightAmount() const { return CurrLightAmount; }
};
