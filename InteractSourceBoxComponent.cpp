// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractSourceBoxComponent.h"

void UInteractSourceBoxComponent::BeginPlay()
{
	OnComponentBeginOverlap.AddDynamic(this, &UInteractSourceBoxComponent::HandleOnComponentBeginOverlap_Implementation);
	OnComponentEndOverlap.AddDynamic(this, &UInteractSourceBoxComponent::HandleOnComponentEndOverlap_Implementation);
}

void UInteractSourceBoxComponent::HandleOnComponentBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	UInteractManagerComponent* InteractManagerComponent = OtherActor->FindComponentByClass<UInteractManagerComponent>();
	if (InteractManagerComponent)
	{
		InteractManagerComponent->AddInteractSource(this);
	}
}

void UInteractSourceBoxComponent::HandleOnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	UInteractManagerComponent* InteractManagerComponent = OtherActor->FindComponentByClass<UInteractManagerComponent>();
	if (InteractManagerComponent)
	{
		InteractManagerComponent->RemoveInteractSource(this);
	}
}