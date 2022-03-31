// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "TimelineCurveVisualizer.generated.h"

UCLASS()
class SPLINTERCELL_API ATimelineCurveVisualizer : public AActor
{
	GENERATED_BODY()

	FTimeline Timeline;
	FVector InitialLocation;
	
public:	
	// Sets default values for this actor's properties
	ATimelineCurveVisualizer();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere)
	float HorizontalDelta;

	UPROPERTY(EditAnywhere)
	float VerticalDelta;

	UPROPERTY(EditAnywhere)
	float Duration;

	UPROPERTY(EditAnywhere)
	UCurveFloat* Curve;

	UPROPERTY(VisibleAnywhere)
	float TimelineValue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TimelineCallback();

};
