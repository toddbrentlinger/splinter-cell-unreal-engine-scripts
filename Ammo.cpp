// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

// Sets default values
AAmmo::AAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Damage = 0.f;

	//DamageType = UDamageType();

}
/*
// Called when the game starts or when spawned
void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/

bool AAmmo::CanApplyHitEffect(AActor* DamagedActor, AController* EventInstigator, const FVector& HitFromDirection, const FHitResult& HitInfo)
{
	return true;
}

void AAmmo::ApplyHitEffect_Implementation(AActor* DamagedActor, AController* EventInstigator, const FVector& HitFromDirection, const FHitResult& HitInfo)
{
	/*if (HitFromDirection && HitInfo)
	{
		UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, HitFromDirection, HitInfo, EventInstigator, this, UDamageType::StaticClass());
	}
	else
	{
		UGameplayStatics::ApplyDamage(DamagedActor, Damage, EventInstigator, this, UDamageType::StaticClass());
	}*/
}

