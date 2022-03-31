// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "InteractInterface.h"
#include "InteractManagerComponent.h"
#include "InteractSourceBoxComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "Interact", editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (BlueprintSpawnableComponent))
class SPLINTERCELL_API UInteractSourceBoxComponent : public UBoxComponent, public IInteractInterface
{
	GENERATED_BODY()

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Interact Interface

	UFUNCTION()
	virtual void HandleOnComponentBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	UFUNCTION()
	virtual void HandleOnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
};
