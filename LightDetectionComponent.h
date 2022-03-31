// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "LightDetector.h"
#include "LightDetectionComponent.generated.h"


UCLASS( Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API ULightDetectionComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULightDetectionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
	virtual void HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
	virtual float GetLightAmountOnDetector(ULightDetector* LightDetector);

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
