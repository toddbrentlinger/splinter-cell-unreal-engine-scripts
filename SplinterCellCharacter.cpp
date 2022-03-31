// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinterCellCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////
// ASplinterCellCharacter

ASplinterCellCharacter::ASplinterCellCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	TurnValue = 0.f;
	LookValue = 0.f;
	MoveForwardValue = 0.f;
	MoveRightValue = 0.f;

	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Set max speeds for each movement state
	/* DELETE
	CrouchSpeedMax = 300.f;
	WalkSpeedMax = 300.f;
	RunSpeedMax = 600.f;
	*/

	// Speed Adjust
	SpeedLevel = 2;
	NumSpeedLevels = 5;
	NumClicksPerSpeedLevelChange = 3;
	SpeedAdjustDuration = 0.5f;
	CrouchSpeedScale = 0.6f;
	MinSpeed = 150.f;
	MaxSpeed = 550.f;
	//CurrentMaxSpeed = MinSpeed + (MaxSpeed - MinSpeed) * 0.6f; // 3 of 5 steps between Min/Max speeds
	//GetCharacterMovement()->MaxWalkSpeed = CurrentMaxSpeed;

	// Movement State
	MovementState = EMovementState::Default;
	MaxTimeCrouchBtnHeldToProne = 0.4f;
	TimeCrouchBtnHeldDown = 0.f;
	bIsCrouchBtnHeldDown = false;

	// Crouch
	CrouchedEyeHeight = 38.f;

	// Smooth Crouch Camera
	SmoothCrouchCameraShiftTimelineValue = 0.f;

	// Health
	MaxHealth = 1.f;
	Health = MaxHealth;

	// Weapon State
	WeaponState = EWeaponState::Unarmed;
	bIsWeaponEquipped = false;
	bAimRightShoulder = true;

	// Camera Mode
	CameraMode = CameraMode::Default;

	// Equip Weapon Camera Adjust
	EquipCameraShiftTimelineValue = 0.f;
	DefaultCameraFOV;
	DefaultBoomTargetArmLength;
	DefaultBoomSocketOffset;
	IronsightCameraFOV = 60.f;
	IronsightBoomTargetArmLength = 75.f;
	IronsightBoomSocketOffset = FVector(23.f, 48.f, 0.f);

	// ---------- Wall Hug Edge Camera Shift ----------

	WallHugEdgeCameraShiftTimelineValue = 0.f;
	WallHugEdgeCameraBoomSocketOffsetY = 100.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->CrouchedHalfHeight = 58.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character
	CameraBoom->SocketOffset = FVector(0.f, 0.f, -30.f);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a post process for the follow camera
	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(FollowCamera);

	VisionModeComponent = CreateDefaultSubobject<UVisionModeComponent>(TEXT("VisionModeComp"));
	LightDetectionManager = CreateDefaultSubobject<ULightDetectionManagerComponent>(TEXT("LightDetectManager"));
	NoiseDetectionComponent = CreateDefaultSubobject<UNoiseDetectionComponent>(TEXT("NoiseDetectionComponent"));
	WallHugComponent = CreateDefaultSubobject<UWallHugComponent>(TEXT("WallHugComponent"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

// Called every frame
void ASplinterCellCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//CrouchTick(DeltaTime);

	// Connect Timeline to DeltaTime
	SmoothCrouchCameraShiftTimeline.TickTimeline(DeltaTime);
	EquipCameraShiftTimeline.TickTimeline(DeltaTime);
	WallHugEdgeCameraShiftTimeline.TickTimeline(DeltaTime);

	// Speed Adjust
	if (SpeedAdjustTimer > 0)
		SpeedAdjustTimer -= DeltaTime;
	else if (CurrClicksDuringDuration != 0)
		CurrClicksDuringDuration = 0;

	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Timeline Playing: %s"), SmoothCrouchCameraShiftTimeline.IsPlaying() ? TEXT("True") : TEXT("False")));

	/*GEngine->AddOnScreenDebugMessage(-1, 0.f,
		FColor::Red, FString::Printf(
			TEXT("Standing Height: %f\nCrouching Height: %f\nAlpha: %f\nBoom: %s"),
			CameraHeightFromCapsuleBottomStanding,
			CameraHeightFromCapsuleBottomCrouching,
			SmoothCrouchCameraShiftTimelineValue,
			*GetCameraBoom()->GetRelativeLocation().ToString()
		)
	);*/
}

void ASplinterCellCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ---------- Smooth Crouch Camera ----------

	CameraHeightFromCapsuleBottomStanding = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + BaseEyeHeight;
	CameraHeightFromCapsuleBottomCrouching = GetCharacterMovement()->CrouchedHalfHeight + CrouchedEyeHeight;
	// If curve is null, define custom curve from (0,0) to (0.5,1)
	if (!CrouchCameraTimelineCurve)
	{
		//CreateZeroToOneCurve(CrouchCameraTimelineCurve, TEXT("CrouchCameraTimelineCurve"), 0.5f);
		
		CrouchCameraTimelineCurve = NewObject<UCurveFloat>(this, TEXT("CrouchCameraTimelineCurve"));
		FKeyHandle FirstKey;
		CrouchCameraTimelineCurve->FloatCurve.AddKey(0.f, 0.f, false, FirstKey);
		CrouchCameraTimelineCurve->FloatCurve.SetKeyInterpMode(FirstKey, ERichCurveInterpMode::RCIM_Cubic);
		CrouchCameraTimelineCurve->FloatCurve.SetKeyTangentMode(FirstKey, ERichCurveTangentMode::RCTM_Auto);
		//CrouchCameraTimelineCurve->FloatCurve.SetKeyTangentWeightMode(FirstKey, ERichCurveTangentWeightMode::RCTWM_WeightedLeave);
		CrouchCameraTimelineCurve->FloatCurve.GetKey(FirstKey).LeaveTangent = 0.f;
		FKeyHandle LastKey;
		CrouchCameraTimelineCurve->FloatCurve.AddKey(0.5f, 1.f, false, LastKey);
		CrouchCameraTimelineCurve->FloatCurve.SetKeyInterpMode(LastKey, ERichCurveInterpMode::RCIM_Cubic);
		CrouchCameraTimelineCurve->FloatCurve.SetKeyTangentMode(LastKey, ERichCurveTangentMode::RCTM_Auto);
		//CrouchCameraTimelineCurve->FloatCurve.SetKeyTangentWeightMode(LastKey, ERichCurveTangentWeightMode::RCTWM_WeightedArrive);
		CrouchCameraTimelineCurve->FloatCurve.GetKey(LastKey).ArriveTangent = 0.f;
		CrouchCameraTimelineCurve->FloatCurve.AutoSetTangents(); // Is this needed since no keys have been set to 'auto'
	}
	// Initialize timeline
	FOnTimelineFloat TimelineCallback;
	SmoothCrouchCameraShiftTimeline.SetPropertySetObject(this); // Is this needed?
	TimelineCallback.BindUFunction(this, FName(TEXT("SmoothCrouchCameraShiftTimelineCallback")));
	SmoothCrouchCameraShiftTimeline.AddInterpFloat(CrouchCameraTimelineCurve, TimelineCallback, FName("SmoothCrouchCameraShiftTimelineValue"));

	// ---------- Speed Adjust ----------

	const float InitialMoveSpeed = CalculateMoveSpeed();
	GetCharacterMovement()->MaxWalkSpeed = InitialMoveSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeedScale * InitialMoveSpeed;

	// ---------- Equip Weapon Camera Adjust ----------

	DefaultCameraFOV = FollowCamera->FieldOfView;
	DefaultBoomSocketOffset = CameraBoom->SocketOffset;
	DefaultBoomTargetArmLength = CameraBoom->TargetArmLength;

	// If curve is null, define custom curve from (0,0) to (0.3,1)
	if (!EquipCameraShiftTimelineCurve)
	{
		//CreateZeroToOneCurve(EquipCameraShiftTimelineCurve, TEXT("EquipCameraShiftTimelineCurve"), 0.3f);

		EquipCameraShiftTimelineCurve = NewObject<UCurveFloat>(this, TEXT("EquipCameraShiftTimelineCurve"));
		
		FKeyHandle FirstKey;
		EquipCameraShiftTimelineCurve->FloatCurve.AddKey(0.f, 0.f, false, FirstKey);
		EquipCameraShiftTimelineCurve->FloatCurve.SetKeyInterpMode(FirstKey, ERichCurveInterpMode::RCIM_Cubic);
		EquipCameraShiftTimelineCurve->FloatCurve.SetKeyTangentMode(FirstKey, ERichCurveTangentMode::RCTM_User);
		//EquipCameraShiftTimelineCurve->FloatCurve.SetKeyTangentWeightMode(FirstKey, ERichCurveTangentWeightMode::RCTWM_WeightedLeave);
		//EquipCameraShiftTimelineCurve->FloatCurve.GetKey(FirstKey).LeaveTangent = 0.f;
		
		FKeyHandle LastKey;
		EquipCameraShiftTimelineCurve->FloatCurve.AddKey(0.3f, 1.f, false, LastKey);
		EquipCameraShiftTimelineCurve->FloatCurve.SetKeyInterpMode(LastKey, ERichCurveInterpMode::RCIM_Cubic);
		EquipCameraShiftTimelineCurve->FloatCurve.SetKeyTangentMode(LastKey, ERichCurveTangentMode::RCTM_User);
		//EquipCameraShiftTimelineCurve->FloatCurve.SetKeyTangentWeightMode(LastKey, ERichCurveTangentWeightMode::RCTWM_WeightedArrive);
		//EquipCameraShiftTimelineCurve->FloatCurve.GetKey(LastKey).ArriveTangent = 0.f;

		//EquipCameraShiftTimelineCurve->FloatCurve.AutoSetTangents(); // Is this needed since no keys have been set to 'auto'
	}
	// Initialize timeline
	FOnTimelineFloat EquipCameraShiftTimelineCallbackInterpFunc;
	EquipCameraShiftTimeline.SetPropertySetObject(this); // Is this needed?
	EquipCameraShiftTimelineCallbackInterpFunc.BindUFunction(this, FName(TEXT("EquipCameraShiftTimelineCallback")));
	EquipCameraShiftTimeline.AddInterpFloat(EquipCameraShiftTimelineCurve, EquipCameraShiftTimelineCallbackInterpFunc, FName("EquipCameraShiftTimelineValue"));

	// ---------- Wall Hug w/ Edge Camera Shift ----------

	// If curve is null, use same curve as EquipCameraShift
	if (!WallHugEdgeCameraShiftTimelineCurve)
	{
		WallHugEdgeCameraShiftTimelineCurve = EquipCameraShiftTimelineCurve;
	}
	// Initialize timeline
	FOnTimelineFloat WallHugEdgeCameraShiftTimelineCallbackInterpFunc;
	WallHugEdgeCameraShiftTimeline.SetPropertySetObject(this); // Is this needed?
	WallHugEdgeCameraShiftTimelineCallbackInterpFunc.BindUFunction(this, FName(TEXT("WallHugEdgeCameraShiftTimelineCallback")));
	WallHugEdgeCameraShiftTimeline.AddInterpFloat(WallHugEdgeCameraShiftTimelineCurve, WallHugEdgeCameraShiftTimelineCallbackInterpFunc, FName("WallHugEdgeCameraShiftTimelineValue"));

	// Bind function to OnChangeOnWallHug
	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, FName(TEXT("HandleOnWallHugChange")));
	WallHugComponent->OnChangeOnWallHug.Add(ScriptDelegate);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASplinterCellCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASplinterCellCharacter::Sprint);
	//PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASplinterCellCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASplinterCellCharacter::CrouchToggle);
	//PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASplinterCellCharacter::HandleCrouchRelease);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASplinterCellCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASplinterCellCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ASplinterCellCharacter::Turn); // &APawn::AddControllerYawInput
	PlayerInputComponent->BindAxis("TurnRate", this, &ASplinterCellCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ASplinterCellCharacter::LookUp); // &APawn::AddControllerPitchInput
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASplinterCellCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("SpeedAdjust", this, &ASplinterCellCharacter::OnSpeedAdjust);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASplinterCellCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASplinterCellCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASplinterCellCharacter::OnResetVR);

	//DECLARE_DELEGATE_OneParam(FHandleWeaponStateSelectDelegate, const EWeaponState);
	//PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ASplinterCellCharacter::HandleWeaponSelectRifle);
}


void ASplinterCellCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASplinterCellCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASplinterCellCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASplinterCellCharacter::Turn(float Value)
{
	TurnValue = Value;
	AddControllerYawInput(TurnValue);
}

void ASplinterCellCharacter::LookUp(float Value)
{
	LookValue = Value;
	AddControllerPitchInput(LookValue);
}

void ASplinterCellCharacter::TurnAtRate(float Rate)
{
	// Assign new TurnValue
	//TurnValue = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASplinterCellCharacter::LookUpAtRate(float Rate)
{
	// Assign new LookValue
	//LookValue = Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


// TODO: Combine MoveForward and MoveRight into single function with second parameter bool bIsRight
void ASplinterCellCharacter::MoveForward(float Value)
{
	// Compare Character forward vector with input vector relative to camera position (Cross or Dot product)
	// If two vectors are more than some angle difference, rotate capsule (with no translation) so Character forward vector aligns with input vector
	// If two vectors are less than or equal to some angle difference, move normally with both rotation and translation.

	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		MoveForwardValue = Value;
	}
}

void ASplinterCellCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);

		MoveRightValue = Value;
	}
}

// Speed Adjust

void ASplinterCellCharacter::OnSpeedAdjust(float Value)
{
	if (!GetCharacterMovement()->IsMovingOnGround() || Value == 0.f)
		return;

	if (Value > 0.f)
	{
		// Reset values if previous clicks in opposite direction
		if (CurrClicksDuringDuration < 0)
		{
			CurrClicksDuringDuration = 0;
			SpeedAdjustTimer = SpeedAdjustDuration;
		}
	}
	else if (Value < 0.f)
	{
		// Reset values if previous clicks in opposite direction
		if (CurrClicksDuringDuration > 0)
		{
			CurrClicksDuringDuration = 0;
			SpeedAdjustTimer = SpeedAdjustDuration;
		}
	}

	// Start timer if not already running
	if (SpeedAdjustTimer <= 0.f)
		SpeedAdjustTimer = SpeedAdjustDuration;

	// Increment/Decrement clicks during duration
	CurrClicksDuringDuration += Value;

	// Check if should change speed level and reset values
	if (CurrClicksDuringDuration >= NumClicksPerSpeedLevelChange)
	{
		if (SpeedLevel < NumSpeedLevels - 1)
		{
			SpeedLevel++;
			CurrClicksDuringDuration = 0;
			SpeedAdjustTimer = SpeedAdjustDuration;
		}
		else
			return;
	}
	else if (CurrClicksDuringDuration <= -1 * NumClicksPerSpeedLevelChange)
	{
		if (SpeedLevel > 0)
		{
			SpeedLevel--;
			CurrClicksDuringDuration = 0;
			SpeedAdjustTimer = SpeedAdjustDuration;
		}
		else
			return;
	}
	else
	{
		return;
	}

	//SpeedLevel += (int32)Value;
	//// Clamp value
	//if (SpeedLevel < 0)
	//	SpeedLevel = 0;
	//else if (SpeedLevel > NumSpeedLevels - 1)
	//	SpeedLevel = NumSpeedLevels - 1;

	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeedScale * CalculateMoveSpeed();
	GetCharacterMovement()->MaxWalkSpeed = CalculateMoveSpeed();

	// ------------------------------------------------------------------------

	/*const float ds = (MaxSpeed - MinSpeed) / 4.f;
	CurrentMaxSpeed = FMath::Clamp(Value * ds + CurrentMaxSpeed, MinSpeed, MaxSpeed);

	if (bIsCrouched)
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMaxSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = CurrentMaxSpeed;*/

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Value: %i\nMove Speed: %f\nNew Speed: %f"), Value, CalculateMoveSpeed(), CurrentMaxSpeed));
}

float ASplinterCellCharacter::CalculateMoveSpeed() const
{
	return MinSpeed + ((float)SpeedLevel / ((float)(NumSpeedLevels - 1))) * (MaxSpeed - MinSpeed);
}

// Sprint - DELETE

//void ASplinterCellCharacter::Sprint()
//{
//	if (GetCharacterMovement()->IsMovingOnGround())
//	{
//		if (bIsCrouched)
//		{
//			UnCrouch();
//		}
//		GetCharacterMovement()->MaxWalkSpeed = RunSpeedMax;
//	}
//}
//
//void ASplinterCellCharacter::StopSprinting()
//{
//	GetCharacterMovement()->MaxWalkSpeed = WalkSpeedMax;
//}

// Crouch

void ASplinterCellCharacter::CrouchToggle()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}
	}
}
/*
void ASplinterCellCharacter::HandleCrouchPress()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bIsCrouchBtnHeldDown = true;
	}
	else 
	{
		bIsCrouchBtnHeldDown = false;
		TimeCrouchBtnHeldDown = 0.f;
	}
}

void ASplinterCellCharacter::HandleCrouchRelease()
{
	bIsCrouchBtnHeldDown = false;
}

void ASplinterCellCharacter::CrouchTick(float DeltaSeconds)
{
	// Check if Character is moving on ground. If not, return.
	if (!GetCharacterMovement()->IsMovingOnGround())
		return;

	if (bIsCrouchBtnHeldDown)
	{
		// Increment time crouch button held down
		TimeCrouchBtnHeldDown += DeltaSeconds;

		// Check if crouch button held down long enough for prone. If not, return.
		if (TimeCrouchBtnHeldDown < MaxTimeCrouchBtnHeldToProne)
			return;

		// Check if already prone
		if (MovementState == EMovementState::Prone)
		{
			// Crouch button held down long enough to go from prone to standing
			CrouchToggle();
			MovementState = EMovementState::Default;
		}
		else
		{
			// Crouch button held down long enough to go to prone
			if (MovementState == EMovementState::Default)
				CrouchToggle();

			MovementState = EMovementState::Prone;
		}

		// Reset variables
		bIsCrouchBtnHeldDown = false;
		TimeCrouchBtnHeldDown = 0.f;
	}
	else
	{
		// Check if crouch btn was previously held down (released this frame). If not, return.
		if (!TimeCrouchBtnHeldDown)
			return;

		// Crouch was held down previously but not long enough for prone
		switch (MovementState)
		{
			case EMovementState::Default:
				MovementState = EMovementState::Crouch;
				CrouchToggle();
				break;
			case EMovementState::Crouch:
				MovementState = EMovementState::Default;
				CrouchToggle();
				break;
			case EMovementState::Prone:
				MovementState = EMovementState::Crouch;
				break;
		}

		TimeCrouchBtnHeldDown = 0.f;
	}
}
*/

// Weapon

void ASplinterCellCharacter::HandleWeaponStateSelect(EWeaponState SelectedWeaponState)
{
	if (SelectedWeaponState == WeaponState)
	{
		WeaponState = EWeaponState::Unarmed;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else 
	{
		WeaponState = SelectedWeaponState;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void ASplinterCellCharacter::HandleWeaponSelectRifle()
{
	HandleWeaponStateSelect(EWeaponState::Rifle);
}

void ASplinterCellCharacter::SmoothCrouchCameraShiftTimelineCallback()
{
	CameraBoom->SetRelativeLocation(
		FVector(0.f, 0.f, UKismetMathLibrary::FInterpEaseInOut(
			CameraHeightFromCapsuleBottomStanding,
			CameraHeightFromCapsuleBottomCrouching,
			SmoothCrouchCameraShiftTimelineValue,
			2.f
		) - GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
	);

	/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
		FString::Printf(TEXT("SmoothCrouchCameraShiftTimelineValue: %f"), SmoothCrouchCameraShiftTimelineValue)
	);*/
}

void ASplinterCellCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	ACharacter::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	//CrouchHalfHeightAdjust = ScaledHalfHeightAdjust;
	SmoothCrouchCameraShiftTimeline.Play();
}

void ASplinterCellCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	ACharacter::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	//CrouchHalfHeightAdjust = ScaledHalfHeightAdjust;
	SmoothCrouchCameraShiftTimeline.Reverse();
}

// Equip Weapon Camera Shift

void ASplinterCellCharacter::EquipCameraShiftTimelineCallback()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, 
		FString::Printf(TEXT("EquipCameraShiftTimelineValue: %f"), EquipCameraShiftTimelineValue)
	);*/

	FollowCamera->FieldOfView = UKismetMathLibrary::Lerp(DefaultCameraFOV, IronsightCameraFOV, EquipCameraShiftTimelineValue);
	CameraBoom->TargetArmLength = UKismetMathLibrary::Lerp(DefaultBoomTargetArmLength, IronsightBoomTargetArmLength, EquipCameraShiftTimelineValue);
	CameraBoom->SocketOffset = UKismetMathLibrary::VLerp(
		DefaultBoomSocketOffset,
		bAimRightShoulder ? IronsightBoomSocketOffset : (IronsightBoomSocketOffset * FVector(1.f, -1.f, 1.f)),
		EquipCameraShiftTimelineValue
	);
}

void ASplinterCellCharacter::PlayEquipCameraShift()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("StartCameraShiftTimeline!"));
	EquipCameraShiftTimeline.Play();
}

void ASplinterCellCharacter::ReverseEquipCameraShift()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ReverseCameraShiftTimeline!"));
	EquipCameraShiftTimeline.Reverse();
}

void ASplinterCellCharacter::WallHugEdgeCameraShiftTimelineCallback()
{
	FVector NewRelativeLocation = CameraBoom->GetRelativeLocation();
	NewRelativeLocation.Y = UKismetMathLibrary::Lerp(0.f, WallHugEdgeCameraBoomSocketOffsetY, WallHugEdgeCameraShiftTimelineValue);
	if (!WallHugComponent->bIsFacingRight)
		NewRelativeLocation.Y *= -1.f;
	CameraBoom->SetRelativeLocation(NewRelativeLocation, true);
}

void ASplinterCellCharacter::PlayWallHugEdgeCameraShift()
{
	WallHugEdgeCameraShiftTimeline.Play();
}

void ASplinterCellCharacter::ReverseWallHugEdgeCameraShift()
{
	WallHugEdgeCameraShiftTimeline.Reverse();
}

void ASplinterCellCharacter::HandleOnWallHugChange(EOnWallHugState OldState, EOnWallHugState NewState)
{
	if (NewState == EOnWallHugState::Edge)
	{
		PlayWallHugEdgeCameraShift();
	}
	else if (OldState == EOnWallHugState::Edge)
	{
		ReverseWallHugEdgeCameraShift();
		return;
	}
}

void ASplinterCellCharacter::CreateZeroToOneCurve(UCurveFloat* CurveFloat, FName Name, float Length)
{
	CurveFloat = NewObject<UCurveFloat>(this, Name);

	FKeyHandle FirstKey;
	CurveFloat->FloatCurve.AddKey(0.f, 0.f, false, FirstKey);
	CurveFloat->FloatCurve.SetKeyInterpMode(FirstKey, ERichCurveInterpMode::RCIM_Cubic);
	CurveFloat->FloatCurve.SetKeyTangentMode(FirstKey, ERichCurveTangentMode::RCTM_Auto);
	//CurveFloat->FloatCurve.SetKeyTangentWeightMode(FirstKey, ERichCurveTangentWeightMode::RCTWM_WeightedLeave);
	CurveFloat->FloatCurve.GetKey(FirstKey).LeaveTangent = 0.f;

	FKeyHandle LastKey;
	CurveFloat->FloatCurve.AddKey(Length, 1.f, false, LastKey);
	CurveFloat->FloatCurve.SetKeyInterpMode(LastKey, ERichCurveInterpMode::RCIM_Cubic);
	CurveFloat->FloatCurve.SetKeyTangentMode(LastKey, ERichCurveTangentMode::RCTM_Auto);
	//CurveFloat->FloatCurve.SetKeyTangentWeightMode(LastKey, ERichCurveTangentWeightMode::RCTWM_WeightedArrive);
	CurveFloat->FloatCurve.GetKey(LastKey).ArriveTangent = 0.f;

	CurveFloat->FloatCurve.AutoSetTangents(); // Is this needed since no keys have been set to 'auto'
}