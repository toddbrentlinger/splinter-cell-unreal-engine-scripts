// Fill out your copyright notice in the Description page of Project Settings.


#include "LightDetectionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LightDetectionManagerComponent.h"

// Sets default values for this component's properties
ULightDetectionComponent::ULightDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULightDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &ULightDetectionComponent::HandleOnComponentBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &ULightDetectionComponent::HandleOnComponentEndOverlap);
}

// Called every frame
//void ULightDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void ULightDetectionComponent::HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HandleOnComponentBeginOverlap Starts"));

	if (OtherActor->FindComponentByClass<ULightDetectionManagerComponent>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Actor has LightDetectionManagerComponent"));
	}
}

void ULightDetectionComponent::HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HandleOnComponentEndOverlap Starts"));
}

float ULightDetectionComponent::GetLightAmountOnDetector(ULightDetector* LightDetector)
{
	return 0.f;
}