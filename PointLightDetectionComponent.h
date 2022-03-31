// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
//#include "LightDetectionComponent.h"
#include "Components/PointLightComponent.h"
//#include "LightDetector.h"
#include "LightDetectionInterface.h"
#include "LightDetectionManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PointLightDetectionComponent.generated.h"

/**
 * Light detection for point lights using a sphere collision.
 */
UCLASS(ClassGroup = "LightDetection", editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "LD Sphere Collision", BlueprintSpawnableComponent))
class SPLINTERCELL_API UPointLightDetectionComponent : public USphereComponent, public ILightDetectionInterface
{
	GENERATED_BODY()

	UPROPERTY()
	UPointLightComponent* PointLightComponent;

	protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	public:

	// Sets default values for this component's properties
	UPointLightDetectionComponent();

	// Light Detection Interface

	UFUNCTION()
	virtual void HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit) override;
	UFUNCTION()
	virtual void HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex) override;
	UFUNCTION()
	virtual float GetLightAmountOnDetector(ULightDetector* LightDetector) override;
	
};
