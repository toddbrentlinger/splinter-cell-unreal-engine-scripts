// Fill out your copyright notice in the Description page of Project Settings.


#include "VisionModeComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UVisionModeComponent::UVisionModeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UVisionModeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to PostProcessComponent on owning Actor
	UPostProcessComponent* PostProcessComponent = GetOwner()->FindComponentByClass<UPostProcessComponent>();
	if (IsValid(PostProcessComponent))
	{
		PostProcessComponentRef = PostProcessComponent;
		// Assign DefaultPostProcess settings to Actor PostProcessComponent settings
		DefaultPostProcess = PostProcessComponent->Settings;
	}	
}

// Called every frame
void UVisionModeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVisionModeComponent::SelectVisionMode(EVisionMode SelectedVisionMode)
{
	// Return if PostProcessComponentRef is invalid OR both new AND old values are Default
	if (!IsValid(PostProcessComponentRef) || (SelectedVisionMode == EVisionMode::Default && VisionMode == EVisionMode::Default))
		return;

	// Play audio clip
	UGameplayStatics::PlaySound2D(this, VisionMode == EVisionMode::Default ? TurnOnVisionModeSound : ChangeVisionModeSound);

	// Set new VisionMode
	VisionMode = SelectedVisionMode == VisionMode ? EVisionMode::Default : SelectedVisionMode;

	// Apply Post Process Settings
	switch (VisionMode)
	{
	case EVisionMode::Night:
		PostProcessComponentRef->Settings = NightVisionPostProcess;
		break;
	case EVisionMode::Thermal:
		PostProcessComponentRef->Settings = ThermalVisionPostProcess;
		break;
	case EVisionMode::EMF:
		PostProcessComponentRef->Settings = EMFVisionPostProcess;
		break;
	case EVisionMode::Default:
	default:
		PostProcessComponentRef->Settings = DefaultPostProcess;
	}
}