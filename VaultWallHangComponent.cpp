// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultWallHangComponent.h"

// Sets default values for this component's properties
UVaultWallHangComponent::UVaultWallHangComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	VaultWallHangState = EVaultWallHangState::None;

	DistanceFromWallEdgeToCheck = 10.f;
	VaultHeightMin = 45.f;
	WallHangHeightMin = 180.f;
	WallHangHeightMax = 300.f;
}

// Called when the game starts
void UVaultWallHangComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = Cast<ACharacter>(GetOwner());

	ActorsToIgnore = { CharacterRef };

	if (!TraceObjectTypes.Num())
	{
		TraceObjectTypes = {
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Vehicle),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible),
		};
	}
}

// Called every frame
void UVaultWallHangComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (VaultWallHangState)
	{
	case EVaultWallHangState::None:
		if (CharacterRef->GetCharacterMovement()->IsFalling())
		{
			TryToVaultWallHang();
		}
		break;
	case EVaultWallHangState::VaultingFromStand:
		break;
	case EVaultWallHangState::VaultingFromWallHang:
		break;
	case EVaultWallHangState::MovingToWallHang:
		break;
	case EVaultWallHangState::WallHanging:
		break;
	default:
		break;
	}
}

void UVaultWallHangComponent::HandleMovementInput(float AxisValue, bool bIsRight)
{
	if (VaultWallHangState != EVaultWallHangState::WallHanging)
		return;
}

bool UVaultWallHangComponent::CanCapsuleFit(const FVector& Center, float Radius, float HalfHeight)
{
	TArray<AActor*> OutActors;
	//UKismetSystemLibrary::DrawDebugCapsule(this, Center, HalfHeight, Radius, FRotator::ZeroRotator, FLinearColor::Blue, 5.f, 1.f);
	return !UKismetSystemLibrary::CapsuleOverlapActors(this, Center, Radius, HalfHeight, TraceObjectTypes, AActor::StaticClass(), ActorsToIgnore, OutActors);
}

bool UVaultWallHangComponent::CanVaultToHit(UCapsuleComponent* Capsule, const FHitResult& Hit, FVector& EndLocation)
{
	// Check the ledge is at the appropriate height

	if (!UKismetMathLibrary::InRange_FloatFloat(Hit.Location.Z - Hit.TraceEnd.Z, VaultHeightMin, WallHangHeightMin, true, false))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Ledge NOT at appropriate height!"));
		return false;
	}

	// Check the surface vaulting to is walkable

	if (Hit.Normal.Z < CharacterRef->GetCharacterMovement()->GetWalkableFloorZ())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Surface NOT walkable!"));
		return false;
	}

	// Make sure there is enough room on top of the ledge

	EndLocation = Hit.Location;
	EndLocation.Z += Capsule->GetScaledCapsuleHalfHeight();

	if (!CanCapsuleFit(EndLocation, Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Capsule can NOT fit!"));
		return false;
	}

	// If reach this point, return true
	return true;
}

bool UVaultWallHangComponent::TryToVaultWallHang()
{
	if (!CharacterRef)
		return false;

	UCapsuleComponent* CapsuleComponent = CharacterRef->GetCapsuleComponent();
	if (!CapsuleComponent)
		return false;

	const FVector CharacterLocation = CharacterRef->GetActorLocation();
	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	FVector Start;
	FVector End;
	FHitResult OutHit;

	// Capsule trace for surfaces in front of Character(1.5X capsule height and 1.5X capsule radius distance in front)

	Start = CharacterLocation;
	Start.Z += CapsuleComponent->GetScaledCapsuleHalfHeight_WithoutHemisphere();
	End = Start + CharacterRef->GetActorForwardVector() * CapsuleRadius * 1.5f;

	UKismetSystemLibrary::CapsuleTraceSingleForObjects(this, Start, End, CapsuleRadius, 1.5f * CapsuleHalfHeight, TraceObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	// Return if NO object in front of character
	if (!OutHit.bBlockingHit)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("No object in front of Character!"));
		return false;
	}

	// There is an object in front of the character

	const float ForwardDistanceToWall = OutHit.Distance;
	const FVector WallLocation = OutHit.ImpactPoint;
	const FVector WallNormalForwardVector = UKismetMathLibrary::GetForwardVector(
		UKismetMathLibrary::MakeRotFromX(OutHit.ImpactNormal)
	);

	// Line trace for height of wall close to edge

	Start = CharacterLocation;
	Start.Z += 2.f * CapsuleHalfHeight;
	Start -= WallNormalForwardVector * (CapsuleRadius + ForwardDistanceToWall + DistanceFromWallEdgeToCheck);
	End = Start;
	End.Z -= 3.f * CapsuleHalfHeight;

	UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, TraceObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FLinearColor::Green, FLinearColor::Blue);

	// Return if NO edge detected
	if (!OutHit.bBlockingHit)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("No edge detected!"));
		return false;
	}

	// There is an edge detected

	const FVector WallEdgeLocation = OutHit.Location;
	const float WallEdgeHeightFromCapsuleBottom = WallEdgeLocation.Z - (CapsuleComponent->GetComponentLocation().Z - CapsuleHalfHeight);

	// Check wall height

	if (WallEdgeHeightFromCapsuleBottom > WallHangHeightMax)
	{
		// Wall Too High
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Wall Too High!"));
	}
	else if (WallEdgeHeightFromCapsuleBottom > WallHangHeightMin)
	{
		// Wall Hang
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Wall Hang!"));

		const FVector WallCornerLocation = FVector(
			WallLocation.X,
			WallLocation.Y,
			WallEdgeLocation.Z
		);

		FVector Center = WallCornerLocation + WallNormalForwardVector * CapsuleRadius;
		Center.Z -= CapsuleHalfHeight;

		UKismetSystemLibrary::DrawDebugCapsule(this, Center, CapsuleHalfHeight, CapsuleRadius, FRotator::ZeroRotator, FLinearColor::White, 5.f, 1.f);

		VaultWallHangState = EVaultWallHangState::MovingToWallHang;
		return true;
	}
	else if (WallEdgeHeightFromCapsuleBottom > VaultHeightMin)
	{
		// Vault
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Wall Vault!"));

		// Line trace for vault end position capsule radius distance away from edge

		Start -= WallNormalForwardVector * (CapsuleRadius - DistanceFromWallEdgeToCheck);
		End = Start;
		End.Z -= 3.f * CapsuleHalfHeight;

		UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, TraceObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true);

		FVector VaultLocation = OutHit.bBlockingHit ? OutHit.Location : OutHit.TraceEnd;

		// Calculate vault height

		const float VaultHeightFromCapsuleBottom = VaultLocation.Z - (CharacterLocation.Z - CapsuleHalfHeight);

		// If vault height less than wall edge height (vault end height is higher than start height)
		if (VaultHeightFromCapsuleBottom < WallEdgeHeightFromCapsuleBottom)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Vault Over Short Wall!."));

			const float HalfHeightOppositeWallSideCheck = 0.5f * (WallEdgeHeightFromCapsuleBottom - VaultHeightFromCapsuleBottom + CapsuleRadius);
			Start = VaultLocation - WallNormalForwardVector * CapsuleRadius;
			Start.Z += HalfHeightOppositeWallSideCheck;
			End = Start + WallNormalForwardVector * CapsuleRadius;

			UKismetSystemLibrary::CapsuleTraceSingleForObjects(this, Start, End, CapsuleRadius, HalfHeightOppositeWallSideCheck, TraceObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true);

			if (!OutHit.bBlockingHit)
				return false;

			const FVector VaultWallOppositeSideLocation = OutHit.ImpactPoint;
			const float VaultWallThickness = UKismetMathLibrary::Vector_Distance2D(VaultWallOppositeSideLocation, WallLocation);

			// Line trace for vault end position capsule radius distance away from new adjusted edge

			Start = CharacterLocation;
			Start.Z += CapsuleHalfHeight;
			Start -= WallNormalForwardVector * (2.f * CapsuleRadius + ForwardDistanceToWall + VaultWallThickness + DistanceFromWallEdgeToCheck);
			End = Start;
			End.Z -= 3.f * CapsuleHalfHeight;

			UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, TraceObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true);
		}
		// Else vault height is more than or equal to wall edge (vault end height is lower than or equal to start height)
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Vault Up!"));
		}

		// If hit location can be vaulted to
		if (CanVaultToHit(CapsuleComponent, OutHit, VaultLocation))
		{
			UKismetSystemLibrary::DrawDebugCapsule(this, VaultLocation, CapsuleHalfHeight, CapsuleRadius, FRotator::ZeroRotator, FLinearColor::White, 5.f, 1.f);

			if (VaultShortWallAnimationMontage)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Start Vault Short Wall Montage!"));
				CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CharacterRef->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

				UAnimInstance* AnimInstance = CharacterRef->GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					// Play Montage
					AnimInstance->Montage_Play(VaultShortWallAnimationMontage);
					//CharacterRef->PlayAnimMontage(VaultShortWallAnimationMontage);

					// Set Blend Out Delegate
					FOnMontageBlendingOutStarted BlendingOutDelegate;
					BlendingOutDelegate.BindUObject(this, &UVaultWallHangComponent::OnMontageBlendingOut);
					AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, VaultShortWallAnimationMontage);
				}
			}

			VaultWallHangState = EVaultWallHangState::VaultingFromStand;
			return true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Failed: Cannot vault to hit!"));
		}
	}
	else
	{
		// Wall Too Low
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Wall Too Low!"));
	}

	// If reach here, could NOT vault or wall hang
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("Failed: Cannot vault or wall hang!"));
	return false;
}

void UVaultWallHangComponent::OnMontageBlendingOut(UAnimMontage* animMontage, bool bInterrupted)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("OnMontageBlendingOut Started!"));
	CharacterRef->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CharacterRef->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}