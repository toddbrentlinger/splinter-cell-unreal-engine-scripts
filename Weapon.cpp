// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("WeaponBodyMesh")));
	RootComponent = Mesh;

	DisplayName = FName(TEXT("Weapon Name"));
	ParentSocketEquipped = FName(TEXT("Right_Hand_Gun_Socket"));
	ParentSocketUnequipped = FName(TEXT("Back_Socket"));

	ActionState = EWeaponActionState::Default;
	EquipState = EWeaponEquipState::Unequip;
	bIsEquipped = false;

	TotalAmmo = 25;
	CurrAmmoInClip = 25;
	RoundsPerClip = 25;

	RateOfFire = 5.f;
	MuzzleVelocity = 100.f;
	EffectiveFiringRange = 5000.f;
	MaximumFiringRange = 10000.f;
	SightMagnificationFOV = 60.f;

	bUseSteadyHandIK = false;

	// Accuracy

	AccuracyFactor = 0.f;
	AccuracyFireImpulse = 0.5f;
	AccuracyFalloffMinSpeed = 150.f;
	AccuracyFalloffMaxSpeed = 550.f;
	AccuracyFalloffMinDeltaRotation = 0.f;
	AccuracyFalloffMaxDeltaRotation = 3.f;
	PrevRotation = FRotator();
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter = Cast<ACharacter>(GetInstigator());
	if (OwningCharacter)
		PrevRotation = OwningCharacter->GetController()->GetDesiredRotation();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwningCharacter)
		return;

	if (bIsEquipped)
	{
		// Speed Factor
		const float SpeedFactor = UKismetMathLibrary::MapRangeClamped(
			OwningCharacter->GetVelocity().Size(),
			AccuracyFalloffMinSpeed, AccuracyFalloffMaxSpeed,
			0.f, 1.f
		);

		// Turn/Look Factor
		const FRotator CurrRotation = OwningCharacter->GetController()->GetDesiredRotation();
		FVector DeltaRotatorVector;
		UKismetMathLibrary::BreakRotator(
			UKismetMathLibrary::NormalizedDeltaRotator(CurrRotation, PrevRotation),
			DeltaRotatorVector.X, DeltaRotatorVector.Y, DeltaRotatorVector.Z
		);
		const float TurnLookFactor = UKismetMathLibrary::MapRangeClamped(
			DeltaRotatorVector.Size(),
			AccuracyFalloffMinDeltaRotation, AccuracyFalloffMaxDeltaRotation,
			0.f, 1.f
		);

		// Set PrevRotation to CurrRotation to be used next frame
		PrevRotation = CurrRotation;

		/*GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red,
			FString::Printf(TEXT("Delta Rotator: %s\nVector: %s"),
				*(DeltaRotator.ToString()),
				*(FVector(CurrRotation.Roll, CurrRotation.Pitch, CurrRotation.Yaw).ToString())
			)
		);*/

		AccuracyFactor = UKismetMathLibrary::FInterpTo(
			AccuracyFactor,
			UKismetMathLibrary::FClamp(SpeedFactor + TurnLookFactor, 0.f, 1.f),
			DeltaTime,
			5.f
		);
	}
}

bool AWeapon::OnFirePrimary_Implementation(const FVector& Start, const FVector& ForwardVector, FHitResult& OutHit)
{
	if (CanFire())
	{
		CurrAmmoInClip--;
		ActionState = EWeaponActionState::FirePrimary;

		// Play Montage
		if (FireAnimationMontage)
		{
			OwningCharacter->PlayAnimMontage(FireAnimationMontage);
		}

		// Perform line trace
		FCollisionQueryParams CollisionParams;
		CollisionParams.bReturnPhysicalMaterial = true;
		//CollisionParams.bDebugQuery = true;
		//FVector Start = AimComponentRef->GetComponentLocation();
		//FVector ForwardVector = UKismetMathLibrary::GetForwardVector(AimComponentRef->GetComponentRotation());
		//FVector End = (ForwardVector * MaximumFiringRange) + Start;
		FVector End = Start + MaximumFiringRange * UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ForwardVector, UKismetMathLibrary::Lerp(0.f, 5.f, AccuracyFactor));
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("Actor Hit: ")) + UKismetSystemLibrary::GetDisplayName(OutHit.GetActor()));
			//DrawDebugLine(GetWorld(), Start, OutHit.Location, FColor::Red, false, 3.f, 0, 2.f);
		}
		else
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.f, 0, 2.f);
		}

		// Add accuracy impulse
		AccuracyFactor = UKismetMathLibrary::FClamp(AccuracyFactor + AccuracyFireImpulse, 0.f, 1.f);

		return true;
	}
	else
	{
		// Play dryfire sound
		if (DryFireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation());
		}
	}
	return false;
}

void AWeapon::OnFirePrimaryHoldStart_Implementation(USceneComponent* NewAimComponent, FHitResult& OutHit)
{
	// Return if aim component parameter is null AND saved aim component ref is still null
	if (!NewAimComponent && !AimComponentRef)
		return;

	// Assign AimComponentRef
	if (AimComponentRef != NewAimComponent)
		AimComponentRef = NewAimComponent;

	// Set timer
	GetWorldTimerManager().SetTimer(
		FireHoldTimerHandle,
		this,
		&AWeapon::OnFirePrimaryWhileHolding,
		(RateOfFire > 0.f) ? (1.f / RateOfFire) : GetWorld()->GetDeltaSeconds(),
		true,
		0.f
	);
}

void AWeapon::OnFirePrimaryHoldEnd_Implementation()
{
	// Clear timer
	GetWorldTimerManager().ClearTimer(FireHoldTimerHandle);
}

void AWeapon::OnFirePrimaryWhileHolding_Implementation()
{
	if (!AimComponentRef)
		return;

	FHitResult OutHit;
	OnFirePrimary(
		AimComponentRef->GetComponentLocation(), 
		UKismetMathLibrary::GetForwardVector(AimComponentRef->GetComponentRotation()), 
		OutHit
	);
}

bool AWeapon::OnFireSecondary_Implementation(const FVector& Start, const FVector& ForwardVector, FHitResult& OutHit)
{
	return false;
}

void AWeapon::OnReload_Implementation()
{
	if (TotalAmmo > 0)
	{
		if (ReloadAnimationMontage && OwningCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadAnimationMontage))
		{
			ActionState = EWeaponActionState::Reload;

			if (bUseSteadyHandIK)
				SteadyHandFABRIKAlpha = 0.f;

			UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
			if (!AnimInstance->Montage_IsPlaying(ReloadAnimationMontage))
			{
				AnimInstance->Montage_Play(ReloadAnimationMontage);
			}
		}

		int AmmoAddedToClip = FMath::Min(RoundsPerClip - CurrAmmoInClip, TotalAmmo);
		TotalAmmo -= AmmoAddedToClip;
		CurrAmmoInClip += AmmoAddedToClip;
	}
}

bool AWeapon::CanFire()
{
	return CurrAmmoInClip > 0;
}

//void AWeapon::FirePrimary_Implementation(FVector& Start, FVector& ForwardVector)
//{
//	OnFirePrimary(Start, ForwardVector);
//}

void AWeapon::OnEquip_Implementation()
{
	if (!bIsEquipped)
		bIsEquipped = true;
}

void AWeapon::OnUnequip_Implementation()
{
	if (bIsEquipped)
		bIsEquipped = false;
}
