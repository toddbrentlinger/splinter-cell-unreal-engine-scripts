// Fill out your copyright notice in the Description page of Project Settings.

#include "WallHugComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UWallHugComponent::UWallHugComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WallHugState = EWallHugState::None;
	OnWallHugState = EOnWallHugState::Idle;

	bIsFacingRight = true;

	MoveToWallDuration = 0.25f;
	//EdgeCheckDistanceFromCapsule = 2.f * CharacterRef->GetCapsuleComponent()->GetScaledCapsuleRadius();

	MoveToWallBegin = FVector::ZeroVector;
	MoveToWallEnd = FVector::ZeroVector;
	MoveToWallAlpha = 0.f;
	MoveToWallRotBegin = FRotator::ZeroRotator;
	MoveToWallRotEnd = FRotator::ZeroRotator;

	WallNormal = FVector::ZeroVector;

	WallHugTraceChannel = ECC_Visibility;
}

void UWallHugComponent::SetOnWallHugState(const EOnWallHugState NewOnWallHugState)
{
	if (OnWallHugState == NewOnWallHugState)
		return;

	//OnChangeOnWallHug(OnWallHugState, NewOnWallHugState);
	OnChangeOnWallHug.Broadcast(OnWallHugState, NewOnWallHugState);

	// Check if starting Edge state
	if (NewOnWallHugState == EOnWallHugState::Edge && OnWallHugState != EOnWallHugState::Edge)
	{
		OnStartWallHugEdge();
	}

	// Check if ending Edge state
	if (NewOnWallHugState != EOnWallHugState::Edge && OnWallHugState == EOnWallHugState::Edge)
	{
		OnStopWallHugEdge();
	}

	OnWallHugState = NewOnWallHugState;
}

//void UWallHugComponent::OnChangeOnWallHug_Implementation(EOnWallHugState PrevState, EOnWallHugState NewState)
//{
//	// ...
//}

// Called when the game starts
void UWallHugComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = Cast<ACharacter>(GetOwner());
	//AnimInstance = CharacterRef->GetMesh()->GetAnimInstance();

	// Turn 180

	BaseMeshRotation = CharacterRef->GetMesh()->GetRelativeRotation();
	
}

// Called every frame
void UWallHugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (WallHugState)
	{
		case EWallHugState::None:
			break;
		case EWallHugState::MovingToWall:
			TickMovingToWall(DeltaTime);
			break;
		case EWallHugState::OnWall:
			TickOnWall(DeltaTime);
			break;
		case EWallHugState::MovingFromWall:
			TickMovingFromWall(DeltaTime);
			break;
		default:
			break;
	}

	/*GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("-----Wall Hug State-----\nWall Hug State: %i\nOn Wall Hug State: %i\nIs Facing Right: %s"),  
		WallHugState,
		OnWallHugState,
		bIsFacingRight ? TEXT("True") : TEXT("False")
	));*/
}

void UWallHugComponent::TickMovingToWall(float DeltaTime)
{
	// Update alpha value
	MoveToWallAlpha = UKismetMathLibrary::FClamp(MoveToWallAlpha + DeltaTime / MoveToWallDuration, 0.f, 1.f);

	// Set Actor location
	CharacterRef->SetActorLocation(UKismetMathLibrary::VLerp(MoveToWallBegin, MoveToWallEnd, MoveToWallAlpha), true);

	// Set Actor rotation
	CharacterRef->SetActorRotation(UKismetMathLibrary::RLerp(MoveToWallRotBegin, MoveToWallRotEnd, MoveToWallAlpha, true));

	// Set Character mesh relative location to move mesh closer to edge of capsule
	// AND to account for Turn180 translation from root motion of montage (15.f for both).
	FVector NewRelativeLocation = CharacterRef->GetMesh()->GetRelativeLocation();
	NewRelativeLocation.X = FMath::Lerp(0.f, -15.f, MoveToWallAlpha);
	NewRelativeLocation.Y = FMath::Lerp(0.f, bIsFacingRight ? 15.f : -15.f, MoveToWallAlpha);
	CharacterRef->GetMesh()->SetRelativeLocation(NewRelativeLocation);

	// Change WallHugState if Character has reached the wall
	if (MoveToWallAlpha >= 1.f)
	{
		WallHugState = EWallHugState::OnWall;
	}
}

void UWallHugComponent::TickMovingFromWall(float DeltaTime)
{
	// Update alpha value
	MoveToWallAlpha = FMath::Clamp(MoveToWallAlpha + DeltaTime / MoveToWallDuration, 0.f, 1.f);

	// Set Actor rotation
	CharacterRef->SetActorRotation(UKismetMathLibrary::RLerp(MoveToWallRotBegin, MoveToWallRotEnd, MoveToWallAlpha, true));

	// Set Character mesh relative location to move mesh away from edge of capsule back to center
	FVector NewMeshLocation = CharacterRef->GetMesh()->GetRelativeLocation();
	NewMeshLocation.X = FMath::Lerp(-15.f, 0.f, MoveToWallAlpha);
	NewMeshLocation.Y = FMath::Lerp(bIsFacingRight ? 15.f : -15.f, 0.f, MoveToWallAlpha);
	CharacterRef->GetMesh()->SetRelativeLocation(NewMeshLocation);

	// Change WallHugState if Character has reached the wall
	if (MoveToWallAlpha >= 1.f)
	{
		WallHugState = EWallHugState::None;

		CharacterRef->GetCharacterMovement()->bOrientRotationToMovement = true;
		CharacterRef->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void UWallHugComponent::TickOnWall(float DeltaTime)
{
	// Line trace for surface behind character
	FHitResult OutHit;
	// There is a surface behind the Character
	if (LineTrace(CharacterRef->GetActorLocation(), 180.f, OutHit))
	{
		WallNormal = OutHit.Normal;

		// Capsule trace for surface directly behind or there's space to wall
		// There is space behind between wall and Character
		if (!UKismetSystemLibrary::CapsuleTraceSingle(
			this,
			CharacterRef->GetCapsuleComponent()->GetComponentLocation(),
			CharacterRef->GetCapsuleComponent()->GetComponentLocation() - WallNormal,
			CharacterRef->GetCapsuleComponent()->GetScaledCapsuleRadius(),
			CharacterRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
			UEngineTypes::ConvertToTraceType(WallHugTraceChannel),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green, 1.f
			))
		{
			// Move character back toward wall, closing the space
			CharacterRef->AddMovementInput(WallNormal, -1.f);
		}

		// Set character rotation to match wall normal
		CharacterRef->SetActorRotation(UKismetMathLibrary::RInterpTo(
			CharacterRef->GetActorRotation(), 
			UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, WallNormal),
			DeltaTime, 10.f)
		);
	}
	// There is NO surface behind the Character
	else
	{
		EndWallHug();
	}
}

bool UWallHugComponent::TryBeginWallHug()
{
	switch (WallHugState)
	{
		case EWallHugState::None:
		{
			FVector ClosestWallNormal = FVector::ZeroVector;
			if (FindClosestWall(CharacterRef->GetActorLocation(), 10, MoveToWallEnd, ClosestWallNormal))
			{
				MoveToWallBegin = CharacterRef->GetActorLocation();
				MoveToWallRotBegin = CharacterRef->GetActorRotation();
				MoveToWallRotEnd = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, ClosestWallNormal);
				MoveToWallAlpha = 0.f;
				WallHugState = EWallHugState::MovingToWall;
				bIsFacingRight = (FVector::CrossProduct(ClosestWallNormal, CharacterRef->GetActorForwardVector()).Z > 0);

				CharacterRef->GetCharacterMovement()->bOrientRotationToMovement = false;
				CharacterRef->GetCharacterMovement()->bUseControllerDesiredRotation = false;

				return true;
			}
		}
			break;
		case EWallHugState::MovingToWall:
			break;
		case EWallHugState::OnWall:
			break;
		case EWallHugState::MovingFromWall:
			break;
		default:
			break;
	}

	return false;
}

bool UWallHugComponent::FindClosestWall(const FVector& LineTraceStart, const int NumLineTraces, FVector& ClosestWallLocation, FVector& ClosestWallNormal)
{
	float ClosestWallDistance = 0.f;
	//FVector ClosestWallLocation;
	//FVector ClosestWallNormal;

	float CurrentWallDistance;
	FVector CurrentWallLocation;
	FVector CurrentWallNormal;

	for (int i = 0; i < NumLineTraces; i++)
	{
		FHitResult OutHit;
		if (LineTrace(LineTraceStart, i * 360.f / NumLineTraces, OutHit))
		{
			CurrentWallDistance = OutHit.Distance;
			CurrentWallLocation = OutHit.Location;
			CurrentWallNormal = OutHit.Normal;

			// If the line trace hit the shortest one yet
			if ((CurrentWallDistance < ClosestWallDistance) || ClosestWallNormal.IsZero())
			{
				ClosestWallDistance = CurrentWallDistance;
				ClosestWallLocation = CurrentWallLocation;
				ClosestWallNormal = CurrentWallNormal;
			}
		}
	}

	return !ClosestWallNormal.IsZero();
}

bool UWallHugComponent::LineTrace(const FVector& Start, float RotationAngleFromForward, FHitResult& OutHit)
{
	//FCollisionQueryParams TraceParams(FName(TEXT("TraceParams")), false, this);
	//FVector End = Start + 100.f * UKismetMathLibrary::RotateAngleAxis(CharacterRef->GetActorForwardVector(), RotationAngleFromForward, FVector::UpVector);
	FVector End = Start + 100.f * CharacterRef->GetActorForwardVector().RotateAngleAxis(RotationAngleFromForward, FVector::UpVector);

	//return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, TraceParams);
	return UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(WallHugTraceChannel), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FLinearColor::Red, FLinearColor::Green, 1.f);
}

void UWallHugComponent::EndWallHug()
{
	MoveToWallRotBegin = CharacterRef->GetActorRotation();
	MoveToWallRotEnd = UKismetMathLibrary::ComposeRotators(
		CharacterRef->GetActorRotation(),
		UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::UpVector, bIsFacingRight ? 45.f : -45.f)
	);
	MoveToWallAlpha = 0.f;
	WallHugState = EWallHugState::MovingFromWall;
	return;

	WallHugState = EWallHugState::None;

	CharacterRef->GetCharacterMovement()->bOrientRotationToMovement = true;
	CharacterRef->GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// Set Character Mesh location from capsule edge back to center
	FVector NewMeshLocation = CharacterRef->GetMesh()->GetRelativeLocation();
	NewMeshLocation.X = 0.f;
	NewMeshLocation.Y = 0.f;
	CharacterRef->GetMesh()->SetRelativeLocation(NewMeshLocation);

	// Set Character rotation
	CharacterRef->SetActorRotation(
		UKismetMathLibrary::ComposeRotators(
			CharacterRef->GetActorRotation(),
			UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::UpVector, bIsFacingRight ? 45.f : -45.f)
		)
	);
}

void UWallHugComponent::HandleMovementInput(float AxisValue, bool bIsRight)
{
	switch (WallHugState)
	{
	case EWallHugState::None:
		break;
	case EWallHugState::MovingToWall:
		break;
	case EWallHugState::OnWall:
		HandleMovementInputOnWall(AxisValue, bIsRight);
		break;
	default:
		break;
	}
}

void UWallHugComponent::HandleMovementInputOnWall(float AxisValue, bool bIsRight)
{
	if (OnWallHugState == EOnWallHugState::Turning180 || WallNormal.IsZero() || !bIsRight)
		return;

	// Check if need to turn 180

	UAnimInstance* AnimInstance = CharacterRef->GetMesh()->GetAnimInstance();
	if (AxisValue < 0.f)
	{
		if (!bIsFacingRight)
		{
			bIsFacingRight = true;

			if (PlayTurn180Montage(
				AnimInstance,
				CharacterRef->bIsCrouched ? Turn180LeftToRightCrouchingMontage : Turn180LeftToRightStandingMontage,
				CharacterRef->bIsCrouched ? Turn180LeftToRightCrouchingRotatorOffset : Turn180LeftToRightStandingRotatorOffset
			))
				return;

			//if (Turn180LeftToRightCrouchingMontage && !AnimInstance->Montage_IsPlaying(Turn180LeftToRightCrouchingMontage))
			//{
			//	OnWallHugState = EOnWallHugState::Turning180;
			//	CharacterRef->GetWorldTimerManager().SetTimer(
			//		Turn180TimerHandle,
			//		this,
			//		&UWallHugComponent::OnTurn180Finished,
			//		AnimInstance->Montage_Play(Turn180LeftToRightCrouchingMontage), // Play montage and return duration
			//		false, // Play only once
			//		-1.f // Play first at InRate
			//	);
			//	return;
			//}
		}
	}
	else if (AxisValue > 0.f)
	{
		if (bIsFacingRight)
		{
			bIsFacingRight = false;

			if (PlayTurn180Montage(
				AnimInstance,
				CharacterRef->bIsCrouched ? Turn180RightToLeftCrouchingMontage : Turn180RightToLeftStandingMontage,
				CharacterRef->bIsCrouched ? Turn180RightToLeftCrouchingRotatorOffset : Turn180RightToLeftStandingRotatorOffset
			))
				return;

			//if (Turn180RightToLeftCrouchingMontage && !CharacterRef->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Turn180RightToLeftCrouchingMontage))
			//{
			//	OnWallHugState = EOnWallHugState::Turning180;
			//	CharacterRef->GetWorldTimerManager().SetTimer(
			//		Turn180TimerHandle,
			//		this,
			//		&UWallHugComponent::OnTurn180Finished,
			//		AnimInstance->Montage_Play(Turn180RightToLeftCrouchingMontage), // Play montage and return duration
			//		false, // Play only once
			//		-1.f // Play first at InRate
			//	);
			//	return;
			//}
		}
	}
	else // Else AxisValue is zero
	{
		SetOnWallHugState(EOnWallHugState::Idle);
		return;
	}

	// Check if at edge of wall

	FHitResult OutHit;
	if (LineTrace(
		CharacterRef->GetActorLocation() + CharacterRef->GetActorRightVector() * CharacterRef->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.f * (bIsFacingRight ? 1.f : -1.f),
		180.f,
		OutHit
	))
	{
		SetOnWallHugState(EOnWallHugState::Moving);
	}
	else
	{
		SetOnWallHugState(EOnWallHugState::Edge);
		return;
	}

	// Move character along wall normal

	FVector NewWorldDirection = FVector::CrossProduct(WallNormal, FVector::UpVector);
	NewWorldDirection.Normalize();
	CharacterRef->AddMovementInput(NewWorldDirection, AxisValue);
}

bool UWallHugComponent::PlayTurn180Montage(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage, FRotator& RotatorOffset)
{
	if (AnimMontage && !AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		/*CharacterRef->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterRef->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);*/

		OnWallHugState = EOnWallHugState::Turning180;

		// Set Relative Rotation
		FHitResult OutHit;
		CharacterRef->GetMesh()->SetRelativeRotation(
			BaseMeshRotation + RotatorOffset,
			false, &OutHit, ETeleportType::TeleportPhysics
		);
		/*CharacterRef->GetMesh()->SetRelativeRotation(
			FRotator(0.f, bIsFacingRight ? 270.f - 90.f : 270.f + 90.f, 0.f), 
			false, &OutHit, ETeleportType::TeleportPhysics
		);*/

		// Set Character Mesh relative location
		FVector NewMeshLocation = CharacterRef->GetMesh()->GetRelativeLocation();
		NewMeshLocation.Y = 0.f;
		CharacterRef->GetMesh()->SetRelativeLocation(NewMeshLocation);

		// Play Montage
		AnimInstance->Montage_Play(AnimMontage);

		// Set Blend Out Delegate
		FOnMontageBlendingOutStarted BlendingOutDelegate;
		BlendingOutDelegate.BindUObject(this, &UWallHugComponent::OnTurn180Finished);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, AnimMontage);

		//CharacterRef->GetWorldTimerManager().SetTimer(
		//	Turn180TimerHandle,
		//	this,
		//	&UWallHugComponent::OnTurn180Finished,
		//	AnimInstance->Montage_Play(AnimMontage), // Play montage and return duration
		//	false, // Play only once
		//	-1.f // Play first at InRate
		//);
		return true;
	}
	return false;
}

void UWallHugComponent::OnTurn180Finished(UAnimMontage* AnimMontage, bool bInterrupted)
{
	/*CharacterRef->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CharacterRef->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);*/

	OnWallHugState = EOnWallHugState::Idle;

	// Set Relative Rotation
	FHitResult OutHit;
	CharacterRef->GetMesh()->SetRelativeRotation(
		BaseMeshRotation,
		false, &OutHit, ETeleportType::TeleportPhysics
	);
	/*CharacterRef->GetMesh()->SetRelativeRotation(
		FRotator(0.f, 270.f, 0.f),
		false, &OutHit, ETeleportType::TeleportPhysics
	);*/

	// Set Character Mesh relative location
	FVector NewMeshLocation = CharacterRef->GetMesh()->GetRelativeLocation();
	NewMeshLocation.Y = bIsFacingRight ? 15.f : -15.f;
	CharacterRef->GetMesh()->SetRelativeLocation(NewMeshLocation);

	// Clear timer
	//CharacterRef->GetWorldTimerManager().ClearTimer(Turn180TimerHandle);
}