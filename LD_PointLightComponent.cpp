// Fill out your copyright notice in the Description page of Project Settings.


#include "LD_PointLightComponent.h"

// Sets default values for this component's properties
ULD_PointLightComponent::ULD_PointLightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("LD Sphere Trigger"));
}


// Called when the game starts
void ULD_PointLightComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULD_PointLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

