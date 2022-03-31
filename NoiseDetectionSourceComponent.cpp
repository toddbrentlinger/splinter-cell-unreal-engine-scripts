// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseDetectionSourceComponent.h"

// Sets default values for this component's properties
UNoiseDetectionSourceComponent::UNoiseDetectionSourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentEnvelopeValue = 0.f;
	HandleOnAudioSingleEnvelopeValueDelegate.BindUFunction(this, "HandleOnAudioSingleEnvelopeValue");
}


// Called when the game starts
void UNoiseDetectionSourceComponent::BeginPlay()
{
	Super::BeginPlay();

	UAudioComponent* ParentAudioComponent = Cast<UAudioComponent>(GetAttachParent());
	if (ParentAudioComponent)
	{
		AudioComponentRef = ParentAudioComponent;
	}
}


// Called every frame
void UNoiseDetectionSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNoiseDetectionSourceComponent::StartSoundDetection()
{
	if (AudioComponentRef && AudioComponentRef->IsPlaying())
	{
		AudioComponentRef->OnAudioSingleEnvelopeValue.Add(HandleOnAudioSingleEnvelopeValueDelegate);
	}
}

void UNoiseDetectionSourceComponent::StopSoundDetection()
{
	AudioComponentRef->OnAudioSingleEnvelopeValue.Remove(HandleOnAudioSingleEnvelopeValueDelegate);
}

void UNoiseDetectionSourceComponent::HandleOnAudioSingleEnvelopeValue(USoundWave* PlayingSoundWave, float EnvelopeValue)
{
	CurrentEnvelopeValue = EnvelopeValue;
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Envelope Value (C++): %f"), EnvelopeValue));
}

bool UNoiseDetectionSourceComponent::IsSoundPlaying() const
{
	return AudioComponentRef->IsPlaying();
}