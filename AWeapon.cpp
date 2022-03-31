// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeapon.h"

// Sets default values
AAWeapon::AAWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Ammo = 25;
	MaxAmmo = 25;
	ActionState = EWeaponActionState::Default;

	TotalAmmo = 25;
	CurrAmmoInClip = 25;
	RoundsPerClip = 25;
}
/*
// Called when the game starts or when spawned
void AAWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/

// TODO: Parameter of raycast hit from center of screen/reticule
void AAWeapon::OnFirePrimary()
{
	if (CurrAmmoInClip > 0)
	{
		CurrAmmoInClip--;
		ActionState = EWeaponActionState::FirePrimary;
	}
	else
	{
		// Play dryfire sound
	}
}

