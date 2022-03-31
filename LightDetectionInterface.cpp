// Fill out your copyright notice in the Description page of Project Settings.


#include "LightDetectionInterface.h"

// Add default functionality here for any ILightDetectionInterface functions that are not pure virtual.
void ILightDetectionInterface::HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	/*ULightDetectionManagerComponent* LightDetectionManager = OtherActor->FindComponentByClass<ULightDetectionManagerComponent>();
	if (LightDetectionManager)
	{
		LightDetectionManager->AddLight(this);
	}*/
}

void ILightDetectionInterface::HandleOnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	/*ULightDetectionManagerComponent* LightDetectionManager = OtherActor->FindComponentByClass<ULightDetectionManagerComponent>();
	if (LightDetectionManager)
	{
		LightDetectionManager->RemoveLight(this);
	}*/
}

float ILightDetectionInterface::GetLightAmountOnDetector(ULightDetector* LightDetector)
{
	return 0.f;
}