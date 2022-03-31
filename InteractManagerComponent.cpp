// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractManagerComponent.h"

// Sets default values for this component's properties
UInteractManagerComponent::UInteractManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (InteractSources.Num())
	{
		UpdateInteractSourcesToDisplay();
	}
}

void UInteractManagerComponent::AddInteractSource(UInteractSourceBoxComponent* InteractSourceComponent)
{
	for (const FInteractSource& InteractSource : InteractSources)
	{
		if (InteractSource.InteractSourceComponent == InteractSourceComponent)
			return;
	}

	InteractSources.Emplace(FInteractSource(InteractSourceComponent));
}

void UInteractManagerComponent::RemoveInteractSource(UInteractSourceBoxComponent* InteractSourceComponent)
{
	for (int32 i = 0; i < InteractSources.Num(); i++)
	{
		if (InteractSources[i].InteractSourceComponent == InteractSourceComponent)
		{
			InteractSources.RemoveAt(i);
			return;
		}
	}
}

void UInteractManagerComponent::UpdateInteractSourcesToDisplay()
{
	for (FInteractSource& InteractSource : InteractSources)
	{
		if (DoesInteractSourceHaveLineOfSight(InteractSource.InteractSourceComponent))
		{
			InteractSource.bHasLineOfSight = true;
			InteractSource.Distance = FVector::Distance(
				GetOwner()->GetActorLocation(), 
				InteractSource.InteractSourceComponent->GetComponentLocation()
			);
		}
		else
		{
			InteractSource.bHasLineOfSight = false;
			// Do not need to set Distance since bHasLineOfSight is false
		}
	}

	SortInteractSources();
}

bool UInteractManagerComponent::DoesInteractSourceHaveLineOfSight(UInteractSourceBoxComponent* InteractSourceComponent)
{
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams = FCollisionQueryParams(FName(TEXT("InteractSource")), true, GetOwner());
	return !GetWorld()->LineTraceSingleByChannel(
		OutHit, 
		InteractSourceComponent->GetComponentLocation(), GetOwner()->GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);
}

void UInteractManagerComponent::SortInteractSources()
{
	int32 i, j, minIndex;

	// One by one move boundary of unsorted subarray
	for (i = 0; i < InteractSources.Num() - 1; i++)
	{
		// Find the minimum element in unsorted array
		minIndex = i;
		for (j = i + 1; j < InteractSources.Num(); j++)
		{
			// If both sources do NOT have line of sight
			if (!InteractSources[j].bHasLineOfSight && !InteractSources[minIndex].bHasLineOfSight)
				continue;
			// Else if both sources have line of sight
			else if (InteractSources[j].bHasLineOfSight && InteractSources[minIndex].bHasLineOfSight)
			{
				// If source distance is less than current min source
				if (InteractSources[j].Distance < InteractSources[minIndex].Distance)
					minIndex = j;
			}
			// Else only one of two sources have line of sight
			// If current source is the one with line of sight
			else if (InteractSources[j].bHasLineOfSight)
			{
				minIndex = j;
			}
		}

		// Swap the found minimum element with the first element
		const FInteractSource& TempInteractSource = InteractSources[minIndex];
		InteractSources[minIndex] = InteractSources[i];
		InteractSources[i] = TempInteractSource;
	}
}