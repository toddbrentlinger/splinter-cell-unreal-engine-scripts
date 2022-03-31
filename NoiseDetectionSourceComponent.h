// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "NoiseDetectionSourceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API UNoiseDetectionSourceComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponentRef;

	TScriptDelegate<FWeakObjectPtr> HandleOnAudioSingleEnvelopeValueDelegate;

public:	
	// Sets default values for this component's properties
	UNoiseDetectionSourceComponent();

	UPROPERTY(BlueprintReadOnly)
	float CurrentEnvelopeValue;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void HandleOnAudioSingleEnvelopeValue(USoundWave* PlayingSoundWave, float EnvelopeValue);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void StartSoundDetection();

	UFUNCTION(BlueprintCallable)
	virtual void StopSoundDetection();

	UFUNCTION(BlueprintCallable)
	virtual bool IsSoundPlaying() const;

	/** Returns AudioComponentRef subobject **/
	FORCEINLINE class UAudioComponent* GetAudioComponentRef() const { return AudioComponentRef; }
};
