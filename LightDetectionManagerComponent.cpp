// Fill out your copyright notice in the Description page of Project Settings.

#include "LightDetectionManagerComponent.h"
#include "LightDetector.h"
//#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
ULightDetectionManagerComponent::ULightDetectionManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void ULightDetectionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Assign any ULightDetector components attached to owning Actor
	GetOwner()->GetComponents<ULightDetector>(LightDetectors);
	
	// If No ULightDetector components attached, add one to owning Actor
	if (!LightDetectors.Num())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Light Detectors!"));
	}

	// Add any Components the Character is already overlapping when play starts

	TArray< UPrimitiveComponent*> OverlappingComponents;
	GetOwner()->GetOverlappingComponents(OverlappingComponents);
	//ILightDetectionInterface* NewLight;
	for (UPrimitiveComponent* OverComp : OverlappingComponents)
	{
		if (OverComp->GetClass()->ImplementsInterface(ULightDetectionInterface::StaticClass()))
		{
			AddLight(OverComp);
			/*NewLight = Cast<ILightDetectionInterface>(OverComp);
			if (NewLight != nullptr)
			{
				AddLight(NewLight);
			}*/
		}
	}

	//TArray<AActor*> OverlappingActors;
	//GetOwner()->GetOverlappingActors(OverlappingActors, ULightDetectionInterface::StaticClass());
	//if (OverlappingActors.Num())
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("There are %i overlapping Actors with interface"), OverlappingActors.Num()));
	//	for (AActor*& OverlapActor : OverlappingActors)
	//	{
	//		ILightDetectionInterface* NewLight = Cast<ILightDetectionInterface>(OverlapActor);
	//		if (NewLight != nullptr)
	//		{
	//			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NewLight is NOT nullptr"));
	//			//AddLight(NewLight);
	//		}
	//	}
	//}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Number of Light Detectors: %i"), LightDetectors.Num()));
}

// Called every frame
void ULightDetectionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Lights.Num())
	{
		float NewTotalLightAmount = CalculateTotalLightAmount();
		SetCurrentLightAmount(NewTotalLightAmount, DeltaTime);
	}
	else
	{
		SetCurrentLightAmount(0.f, DeltaTime);
	}
}

float ULightDetectionManagerComponent::CalculateTotalLightAmount()
{
	float LightFromDetector;
	float TotalLightAmount = 0.f;
	ILightDetectionInterface* LightInterface;
	//for (ILightDetectionInterface*& Light : Lights) {
	//for (const TScriptInterface<ILightDetectionInterface> Light : Lights) {
	for (UObject* Light : Lights) 
	{
		for (ULightDetector* Detector : LightDetectors)
		{
			//LightFromDetector = Light->GetLightAmountOnDetector(Detector);
			LightInterface = Cast<ILightDetectionInterface>(Light);
			if (!LightInterface)
				continue;
			LightFromDetector = LightInterface->GetLightAmountOnDetector(Detector);
			if (LightFromDetector > 0.f)
			{
				TotalLightAmount += LightFromDetector;
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Detectors: %i\nLights: %i\nTotalLightAmount: %f"), LightDetectors.Num(), Lights.Num(), TotalLightAmount));
	return TotalLightAmount;
}

void ULightDetectionManagerComponent::SetCurrentLightAmount(float NewCurrLightAmount, float DeltaTime)
{
	if (!CurrLightAmount && !NewCurrLightAmount)
		return;

	CurrLightAmount = FMath::FInterpTo(CurrLightAmount, NewCurrLightAmount, DeltaTime, 15.f);
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("%f"), CurrLightAmount));
}

void ULightDetectionManagerComponent::AddLight(UObject* NewLight)
{
	if (!Lights.Contains(NewLight))
	{
		Lights.Add(NewLight);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Light Added"));
	}
}

void ULightDetectionManagerComponent::RemoveLight(UObject* NewLight)
{
	Lights.Remove(NewLight);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Light Removed"));
}