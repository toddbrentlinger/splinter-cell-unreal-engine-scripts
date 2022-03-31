// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseDetectionSourceComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/KismetMathLibrary.h"
#include "NoiseDetectionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API UNoiseDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoiseDetectionComponent();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise Detection")
	UAISense* NoiseDetectSense;*/

	UPROPERTY(BlueprintReadWrite, Category="Noise Detection|Ambient Noise")
	TArray<UNoiseDetectionSourceComponent*> AmbientNoiseSourcesToDetect;

	UPROPERTY(BlueprintReadWrite, Category = "Noise Detection|Ambient Noise")
	float AmbientAmplitude;

	UPROPERTY(BlueprintReadWrite, Category = "Noise Detection|Self Noise")
	float SelfAmplitudeInterp;

	UPROPERTY(BlueprintReadWrite, Category = "Noise Detection|Self Noise")
	float SelfAmplitudeTotal;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual float CalculateAmplitudeWithFalloff(UNoiseDetectionSourceComponent* NoiseSource);

	UFUNCTION(BlueprintCallable)
	virtual void HandleOnAudioSingleEnvelopeValueForSelfNoise(USoundWave* SoundWave, float EnvelopeValue);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void HandleOnTargetPerceptionUpdated(const AActor* Actor, const FAIStimulus& Stimulus);
};
