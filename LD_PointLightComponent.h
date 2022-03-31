// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "LD_PointLightComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API ULD_PointLightComponent : public USceneComponent
{
	GENERATED_BODY()

	USphereComponent* SphereComponent;

public:	
	// Sets default values for this component's properties
	ULD_PointLightComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Returns SphereComponent subobject **/
	FORCEINLINE class USphereComponent* GetSphereComponent() const { return SphereComponent; }
};
