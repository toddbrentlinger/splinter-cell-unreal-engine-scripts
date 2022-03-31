// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseDetectionComponent.h"

// Sets default values for this component's properties
UNoiseDetectionComponent::UNoiseDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AmbientAmplitude = 0.f;
	SelfAmplitudeInterp = 0.f;
	SelfAmplitudeTotal = 0.f;
}

// Called when the game starts
void UNoiseDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UNoiseDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (AmbientNoiseSourcesToDetect.Num())
	{
		float MaxAmplitude = 0.f;
		float TempAmplitude;
		for (int32 Index = AmbientNoiseSourcesToDetect.Num() - 1; Index >= 0; Index--)
		{
			if (AmbientNoiseSourcesToDetect[Index]->GetAudioComponentRef()->IsPlaying())
			{
				TempAmplitude = CalculateAmplitudeWithFalloff(AmbientNoiseSourcesToDetect[Index]);
				if (TempAmplitude > MaxAmplitude)
					MaxAmplitude = TempAmplitude;
			}
			else
			{
				AmbientNoiseSourcesToDetect[Index]->StopSoundDetection();
				AmbientNoiseSourcesToDetect.RemoveAt(Index);
			}
		}
		AmbientAmplitude = UKismetMathLibrary::FInterpTo(AmbientAmplitude, MaxAmplitude, DeltaTime, 5.f);
	}
	else if (AmbientAmplitude > 0.f)
	{
		AmbientAmplitude = UKismetMathLibrary::FInterpTo(AmbientAmplitude, 0.f, DeltaTime, 5.f);
	}

	// Self Noise
	if (SelfAmplitudeInterp > 0.f || SelfAmplitudeTotal > 0.f)
	{
		SelfAmplitudeInterp = UKismetMathLibrary::FInterpTo(SelfAmplitudeInterp, SelfAmplitudeTotal, DeltaTime, 5.f);
		SelfAmplitudeTotal = 0.f;
	}

	/*GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red,
		FString::Printf(
			TEXT("Amplitude (Ambient) (C++): %f\nAmplitude (Self) (C++): %f"),
			AmbientAmplitude, SelfAmplitudeInterp
		)
	);*/
}

void UNoiseDetectionComponent::HandleOnTargetPerceptionUpdated(const AActor* Actor, const FAIStimulus& Stimulus)
{
	/*GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
		FString::Printf(
			TEXT("Stimulus.Type (C++): %s"),
			*(Stimulus.Type.Name).ToString()
		)
	);*/

	// Return if NOT Hearing Sense
	// TODO: Could add Tag 'NoiseDetectAmbient' to each noise event
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Hearing>())
		return;
	/*if (!NoiseDetectSense || (NoiseDetectSense->GetSenseID() != Stimulus.Type))
		return;*/

	TArray<UActorComponent*> NoiseSourceComponents;
	Actor->GetComponents(UNoiseDetectionSourceComponent::StaticClass(), NoiseSourceComponents);
	if (NoiseSourceComponents.Num())
	{
		for (auto Iter = NoiseSourceComponents.CreateIterator(); Iter; ++Iter)
		{
			UNoiseDetectionSourceComponent* NoiseSource = Cast<UNoiseDetectionSourceComponent>(*Iter);
			if (!AmbientNoiseSourcesToDetect.Contains(NoiseSource))
			{
				AmbientNoiseSourcesToDetect.Emplace(NoiseSource);
				NoiseSource->StartSoundDetection();
			}
		}
	}
}

float UNoiseDetectionComponent::CalculateAmplitudeWithFalloff(UNoiseDetectionSourceComponent* NoiseSource)
{
	FVector NoiseSourceLocation = NoiseSource->GetComponentLocation();
	float NoiseSourceEnvelope = NoiseSource->CurrentEnvelopeValue;
	UAudioComponent* AudioComponent = NoiseSource->GetAudioComponentRef();
	if (AudioComponent)
	{
		const FSoundAttenuationSettings* AttenuationSettings = AudioComponent->GetAttenuationSettingsToApply();
		const EAttenuationDistanceModel AttenuationFunction = AttenuationSettings->DistanceAlgorithm;
		const TEnumAsByte<EAttenuationShape::Type> AttenuationShape = AttenuationSettings->AttenuationShape;
		const FVector AttenuationShapeExtents = AttenuationSettings->AttenuationShapeExtents;
		const float FalloffDistance = AttenuationSettings->FalloffDistance;

		// Falloff for Sphere attenuation

		float Distance = UKismetMathLibrary::Vector_Distance(NoiseSourceLocation, GetOwner()->GetActorLocation());
		float FalloffRatio = (Distance < FalloffDistance + AttenuationShapeExtents.X) ? Distance / AttenuationShapeExtents.X : -1.f;
		if (FalloffRatio > 1.f)
		{
			return NoiseSourceEnvelope / UKismetMathLibrary::Square(FalloffRatio);
		}
		else if (FalloffRatio >= 0.f)
		{
			return NoiseSourceEnvelope;
		}
	}
	return 0.f;
}

void UNoiseDetectionComponent::HandleOnAudioSingleEnvelopeValueForSelfNoise(USoundWave* SoundWave, float EnvelopeValue)
{
	if (EnvelopeValue > SelfAmplitudeTotal)
	{
		SelfAmplitudeTotal = EnvelopeValue;
	}
}
