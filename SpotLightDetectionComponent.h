// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "LightDetectionInterface.h"
#include "Components/SpotLightComponent.h"
#include "LightDetectionManagerComponent.h"
#include "SpotLightDetectionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "LightDetection", editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "LD Spot Light Collision", BlueprintSpawnableComponent))
class SPLINTERCELL_API USpotLightDetectionComponent : public UBoxComponent, public ILightDetectionInterface
{
	GENERATED_BODY()
	
	UPROPERTY()
	USpotLightComponent* SpotLightComponent;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float CalcChordLength(float Radius, float Angle) const;

public:

	// Sets default values for this component's properties
	USpotLightDetectionComponent();

	// Light Detection Interface

	UFUNCTION()
	virtual void HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit) override;
	UFUNCTION()
	virtual void HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex) override;
	UFUNCTION()
	virtual float GetLightAmountOnDetector(ULightDetector* LightDetector) override;
};
