// Fill out your copyright notice in the Description page of Project Settings.


#include "SpotLightDetectionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

USpotLightDetectionComponent::USpotLightDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	/*SpotLightComponent = Cast<USpotLightComponent>(GetAttachParent());
	if (SpotLightComponent != nullptr)
	{
		float HalfRadius = 0.5f * SpotLightComponent->AttenuationRadius;
		float ChordLength = CalcChordLength(SpotLightComponent->AttenuationRadius, SpotLightComponent->OuterConeAngle);
		SetRelativeLocation(FVector(HalfRadius, 0.f, 0.f));
		SetBoxExtent(FVector(HalfRadius, ChordLength, ChordLength));
	}*/
}

void USpotLightDetectionComponent::BeginPlay()
{
	SpotLightComponent = Cast<USpotLightComponent>(GetAttachParent());
	if (SpotLightComponent != nullptr)
	{
		float HalfRadius = 0.5f * SpotLightComponent->AttenuationRadius;
		float ChordLength = CalcChordLength(SpotLightComponent->AttenuationRadius, SpotLightComponent->OuterConeAngle);
		SetRelativeLocation(FVector(HalfRadius, 0.f, 0.f));
		SetBoxExtent(FVector(HalfRadius, ChordLength, ChordLength));
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attenuation Radius: %f"), PointLightComponent->AttenuationRadius));

	OnComponentBeginOverlap.AddDynamic(this, &USpotLightDetectionComponent::HandleOnComponentBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USpotLightDetectionComponent::HandleOnComponentEndOverlap);
}

void USpotLightDetectionComponent::HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	ULightDetectionManagerComponent* LightDetectionManager = OtherActor->FindComponentByClass<ULightDetectionManagerComponent>();
	if (LightDetectionManager)
	{
		LightDetectionManager->AddLight(this);
	}
}

void USpotLightDetectionComponent::HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	ULightDetectionManagerComponent* LightDetectionManager = OtherActor->FindComponentByClass<ULightDetectionManagerComponent>();
	if (LightDetectionManager)
	{
		LightDetectionManager->RemoveLight(this);
	}
}

float USpotLightDetectionComponent::GetLightAmountOnDetector(ULightDetector* LightDetector)
{
	if (SpotLightComponent == nullptr)
		return 0.f;

	const FVector LightDetectorLocation = LightDetector->GetComponentLocation();
	const FVector SpotLightLocation = SpotLightComponent->GetComponentLocation();

	// If Detector is NOT within spotlight cone, return 0.f
	FVector BetweenVector = (LightDetectorLocation - SpotLightLocation);
	BetweenVector.Normalize();
	const float DotProduct = FVector::DotProduct(BetweenVector, SpotLightComponent->GetForwardVector());
	const float Angle = UKismetMathLibrary::DegAcos(DotProduct);
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("DotProduct: %f\tAngle: %f"), DotProduct, Angle));
	//DrawDebugLine(GetWorld(), SpotLightLocation, LightDetectorLocation, FColor::Yellow, false, 0.f, 0, 5.f);
	if (Angle > SpotLightComponent->OuterConeAngle)
		return 0.f;

	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	if (!GetWorld()->LineTraceSingleByChannel(OutHit, SpotLightLocation, LightDetectorLocation, ECC_Visibility, CollisionParams))
	{
		float Alpha = UKismetMathLibrary::MapRangeClamped(
			(LightDetectorLocation - SpotLightLocation).Size(),
			SpotLightComponent->AttenuationRadius,
			0.f, 0.f, 1.f
		);

		return UKismetMathLibrary::FInterpEaseInOut(0.f, SpotLightComponent->Intensity, Alpha, 2.0f);
	}

	// If reach here, return zero light amount
	return 0.f;
}

float USpotLightDetectionComponent::CalcChordLength(float Radius, float Angle) const
{
	return 2.f * Radius * UKismetMathLibrary::DegSin(0.5f * Angle);
}