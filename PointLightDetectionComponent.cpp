// Fill out your copyright notice in the Description page of Project Settings.


#include "PointLightDetectionComponent.h"

UPointLightDetectionComponent::UPointLightDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//PointLightComponent = Cast<UPointLightComponent>(GetAttachParent());
	//if (PointLightComponent != nullptr)
	//{
	//	//InitSphereRadius(PointLightComponent->AttenuationRadius);
	//	//USphereComponent::SetSphereRadius(PointLightComponent->AttenuationRadius);
	//	SphereRadius = PointLightComponent->AttenuationRadius;
	//}
}

void UPointLightDetectionComponent::BeginPlay()
{
	PointLightComponent = Cast<UPointLightComponent>(GetAttachParent());
	if (PointLightComponent != nullptr)
	{
		SetSphereRadius(PointLightComponent->AttenuationRadius);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attenuation Radius: %f"), PointLightComponent->AttenuationRadius));

	OnComponentBeginOverlap.AddDynamic(this, &UPointLightDetectionComponent::HandleOnComponentBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UPointLightDetectionComponent::HandleOnComponentEndOverlap);
}

void UPointLightDetectionComponent::HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Begin Overlap"));

	ULightDetectionManagerComponent* LightDetectionManager = OtherActor->FindComponentByClass<ULightDetectionManagerComponent>();
	if (LightDetectionManager)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Actor has LightDetectionManagerComponent"));
		//LightDetectionManager->TestAdd();
		//LightDetectionManager->AddLight(Cast<ILightDetectionInterface>(this));
		LightDetectionManager->AddLight(this);
	}
}

void UPointLightDetectionComponent::HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("End Overlap"));

	ULightDetectionManagerComponent* LightDetectionManager = OtherActor->FindComponentByClass<ULightDetectionManagerComponent>();
	if (LightDetectionManager)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Actor has LightDetectionManagerComponent"));
		//LightDetectionManager->TestRemove();
		LightDetectionManager->RemoveLight(this);
	}
}

float UPointLightDetectionComponent::GetLightAmountOnDetector(ULightDetector* LightDetector)
{
	if (PointLightComponent == nullptr)
		return 0.f;

	const FVector LightDetectorLocation = LightDetector->GetComponentLocation();
	const FVector PointLightLocation = PointLightComponent->GetComponentLocation();
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Distance: %f"), (LightDetectorLocation - PointLightLocation).Size()));
	if (!GetWorld()->LineTraceSingleByChannel(OutHit, PointLightLocation, LightDetectorLocation, ECC_Visibility, CollisionParams))
	{
		// Line of sight between light and detector is NOT blocked
		/*if (!OutHit.bBlockingHit)
		{
			float Alpha = UKismetMathLibrary::MapRangeClamped(
				(LightDetectorLocation - PointLightLocation).Size(),
				PointLightComponent->AttenuationRadius,
				0.f, 0.f, 1.f
			);
			
			return UKismetMathLibrary::FInterpEaseInOut(0.f, PointLightComponent->Intensity, Alpha, 2.0f);
		}*/
		float Alpha = UKismetMathLibrary::MapRangeClamped(
			(LightDetectorLocation - PointLightLocation).Size(),
			PointLightComponent->AttenuationRadius,
			0.f, 0.f, 1.f
		);

		return UKismetMathLibrary::FInterpEaseInOut(0.f, PointLightComponent->Intensity, Alpha, 2.0f);
	}

	// If reach here, return zero light amount
	return 0.f;
}