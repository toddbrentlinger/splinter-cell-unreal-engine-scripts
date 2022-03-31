// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.generated.h"

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

USTRUCT(BlueprintType)
struct FWeaponAttachment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DisplayName;

	// Ammo* AmmoType;
	// void OnApplyAttachment(); - apply any changes to weapon properties if necessary
	// void OnRemoveAttachment(); - reverse changes to weapon properties if necessary
};

UCLASS()
class SPLINTERCELL_API AWeapon : public AActor
{
	GENERATED_BODY()

	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACharacter* OwningCharacter;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AimComponentRef;

	FTimerHandle FireHoldTimerHandle;
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FName ParentSocketEquipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FName ParentSocketUnequipped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FTransform RelativeTransformEquipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FTransform RelativeTransformUnequipped;

	UPROPERTY(BlueprintReadWrite)
	uint32 bIsEquipped : 1;

	// Ammo

	/** Total number of rounds for weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int TotalAmmo;
	/** Number of rounds currently in magazine/clip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int CurrAmmoInClip;
	/** Number of rounds per magazine/clip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int RoundsPerClip;
	/** Standary ammo type */
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	AAmmo* StandardAmmoType;*/

	// Weapon Fire Properties

	/* rounds/sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RateOfFire;
	/* meters/sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MuzzleVelocity;
	/* meters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EffectiveFiringRange;
	/* meters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumFiringRange;

	// Camera FOV when aiming-down-sights
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightMagnificationFOV;

	// States

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponActionState ActionState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponEquipState EquipState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = IK)
	uint32 bUseSteadyHandIK : 1;

	UPROPERTY(BlueprintReadWrite, Category = IK)
	float SteadyHandFABRIKAlpha;

	// Animation Montages

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* EquipAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* ReloadAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* FireAnimationMontage;

	// Audio

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	USoundBase* DryFireSound;

	// ---------- Accuracy ----------

	UPROPERTY(BlueprintReadWrite, Category = Accuracy)
	float AccuracyFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Accuracy)
	float AccuracyFireImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy|Speed")
	float AccuracyFalloffMinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy|Speed")
	float AccuracyFalloffMaxSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Accuracy|Rotation")
	FRotator PrevRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy|Rotation")
	float AccuracyFalloffMinDeltaRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy|Rotation")
	float AccuracyFalloffMaxDeltaRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual bool CanFire();

	/*UFUNCTION(BlueprintNativeEvent)
	void FirePrimary(UPARAM(ref) FVector& Start, UPARAM(ref) FVector& ForwardVector);
	virtual void FirePrimary_Implementation(UPARAM(ref) FVector& Start, UPARAM(ref) FVector& ForwardVector);*/

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnFirePrimary(const FVector& Start, const FVector& ForwardVector, FHitResult& OutHit);
	virtual bool OnFirePrimary_Implementation(const FVector& Start, const FVector& ForwardVector, FHitResult& OutHit);
	//virtual bool OnFirePrimary(UPARAM(ref) FVector& Start, UPARAM(ref) FVector& ForwardVector);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFirePrimaryHoldStart(USceneComponent* NewAimComponent, FHitResult& OutHit);
	virtual void OnFirePrimaryHoldStart_Implementation(USceneComponent* NewAimComponent, FHitResult& OutHit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFirePrimaryHoldEnd();
	virtual void OnFirePrimaryHoldEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFirePrimaryWhileHolding();
	virtual void OnFirePrimaryWhileHolding_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnFireSecondary(const FVector& Start, const FVector& ForwardVector, FHitResult& OutHit);
	virtual bool OnFireSecondary_Implementation(const FVector& Start, const FVector& ForwardVector, FHitResult& OutHit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnReload();
	virtual void OnReload_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEquip();
	virtual void OnEquip_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnUnequip();
	virtual void OnUnequip_Implementation();

	// ---------- FORCEINLINE ----------

	FORCEINLINE class ACharacter* GetOwningCharacter() const { return OwningCharacter; }
	FORCEINLINE class USceneComponent* GetAimComponentRef() const { return AimComponentRef; }
};
