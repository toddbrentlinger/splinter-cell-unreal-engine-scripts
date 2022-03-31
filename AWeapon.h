// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "AWeapon.generated.h"
/*
UENUM(BlueprintType)
enum class EWeaponActionState : uint8
{
	Default UMETA(DisplayName = "Default"),
	FirePrimary UMETA(DisplayName = "FirePrimary"),
	FirePrimaryHold UMETA(DisplayName = "FirePrimaryHold"),
	FireSecondary UMETA(DisplayName = "FireSecondary"),
	Reload UMETA(DisplayName = "Reload"),
};

UENUM(BlueprintType)
enum class EWeaponEquipState : uint8
{
	Unequip UMETA(DisplayName = "Unequip"),
	Equip UMETA(DisplayName = "Equip"),
	IsUnequipping UMETA(DisplayName = "IsUnequipping"),
	IsEquipping UMETA(DisplayName = "IsEquipping"),
};
*/
UCLASS()
class SPLINTERCELL_API AAWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAWeapon();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Hip")
	UAnimMontage* ReloadHipAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Hip")
	UAnimMontage* FireHipAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Hip")
	UAnimMontage* EquipHipAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Ironsights")
	UAnimMontage* ReloadIronsightsAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Ironsights")
	UAnimMontage* FireIronsightsAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int TotalAmmo;
	/** Number of rounds currently in magazine/clip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int CurrAmmoInClip;
	/** Number of rounds per magazine/clip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int RoundsPerClip;

	/* rounds/sec */
	float RateOfFire;
	/* meters/sec */
	float MuzzleVelocity;
	/* meters */
	float EffectiveFiringRange;
	/* meters */
	float MaximumFiringRange;

	float SightMagnification; // default 1.f

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponActionState ActionState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponEquipState EquipState;

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	virtual void OnFirePrimary();

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
