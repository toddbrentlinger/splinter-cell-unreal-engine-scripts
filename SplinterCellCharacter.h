// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "VisionModeComponent.h"
#include "LightDetectionManagerComponent.h"
#include "NoiseDetectionComponent.h"
#include "WallHugComponent.h"
#include "SplinterCellCharacter.generated.h"

UENUM(BlueprintType)
enum EWeaponState
{
	Unarmed UMETA(DisplayName = "Unarmed"),
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "Rifle"),
	Throwable UMETA(DisplayName = "Throwable"),
};

UENUM(BlueprintType)
enum EMovementState
{
	Default UMETA(DisplayName = "Default"),
	Crouch UMETA(DisplayName = "Crouch"),
	Prone UMETA(DisplayName = "Prone"),
	WallHug UMETA(DisplayName = "Wall Hug"),
	WallHugCrouch UMETA(DisplayName = "Wall Hug Crouch"),
	Vault UMETA(DisplayName = "Vault"),
	WallHang UMETA(DisplayName = "Wall Hang"),
};

UENUM(BlueprintType)
enum class CameraMode : uint8
{
	Default UMETA(DisplayName = "Default"),
	EEV UMETA(DisplayName = "EEV"),
	StickyCamera UMETA(DisplayName = "Sticky Camera"),
	OpticCable UMETA(DisplayName = "Optic Cable"),
};

UCLASS(config=Game)
class ASplinterCellCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Post Process for follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* PostProcess;

	/** Vision Mode Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UVisionModeComponent* VisionModeComponent;

	/** Light Detection Manager Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULightDetectionManagerComponent* LightDetectionManager;

	/** Noise Detection Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UNoiseDetectionComponent* NoiseDetectionComponent;

	/** Wall Hug Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWallHugComponent* WallHugComponent;

	// ---------- Speed Adjust ----------

	/** Current speed level (ranges from 0 to NumSpeedLevels - 1). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust", meta = (AllowPrivateAccess = "true"))
	int32 SpeedLevel;

	// ---------- Smooth Crouch Camera ----------

	/** Smooth Crouch Camera Shift Timeline */
	UPROPERTY()
	FTimeline SmoothCrouchCameraShiftTimeline;

	// ---------- Equip Weapon Camera Shift ----------

	/** Equip Camera Shift Timeline */
	UPROPERTY()
	FTimeline EquipCameraShiftTimeline;

	// ---------- Wall Hug Edge Camera Shift ----------

	/** Wall Hug Edge Camera Shift Timeline */
	UPROPERTY()
	FTimeline WallHugEdgeCameraShiftTimeline;

protected:

	// ---------- Equip Weapon Camera Shift ----------

	/** Camera default field-of-view (FOV). */
	UPROPERTY(BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	float DefaultCameraFOV;

	/** Camera boom default target arm length. */
	UPROPERTY(BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	float DefaultBoomTargetArmLength;

	/** Camera boom default socket offset. */
	UPROPERTY(BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	FVector DefaultBoomSocketOffset;

	// ---------- Smooth Crouch Camera ----------

	/** Camera height from bottom of capsule while Character is standing */
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Crouch")
	float CameraHeightFromCapsuleBottomStanding;

	/** Camera height from bottom of capsule while Character is crouching */
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Crouch")
	float CameraHeightFromCapsuleBottomCrouching;

public:

	ASplinterCellCharacter();

	/** Axis value from Player input to turn (yaw) for current frame. */
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float TurnValue;

	/** Axis value from Player input to look (pitch) for current frame. */
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float LookValue;

	/** Axis value from Player input to move forward/backward (X-axis) for current frame. */
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float MoveForwardValue;

	/** Axis value from Player input to move left/right (Y-axis) for current frame.  */
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float MoveRightValue;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// ---------- Speeds ----------

	/* DELETE
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Movement|Crouch")
	float CrouchSpeedMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float WalkSpeedMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float RunSpeedMax;
	*/

	// ---------- Speed Adjust ----------

	/** Total number of speed levels. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust")
	int32 NumSpeedLevels;

	/** Number of clicks in same direction to change the current speed level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust")
	int32 NumClicksPerSpeedLevelChange;

	/** Amount of time for the total number of clicks to occur to change the current speed level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust")
	float SpeedAdjustDuration;

	/** Speed at lowest speed level while standing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust")
	float MinSpeed;

	/** Speed at highest speed level while standing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust")
	float MaxSpeed;

	/** Scale applied to speed when crouching (0 < n <= 1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|SpeedAdjust")
	float CrouchSpeedScale;

	/** Timer to record number of clicks in the same direction to change speed level. */
	UPROPERTY(BlueprintReadWrite, Category = "Movement|SpeedAdjust")
	float SpeedAdjustTimer;

	/** Counter for number of clicks in same direction during the specified duration to change the speed level. */
	UPROPERTY(BlueprintReadWrite, Category = "Movement|SpeedAdjust")
	int32 CurrClicksDuringDuration;

	// ---------- Health ----------

	/** Current health amount. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float Health;

	/** Maximum health amount (full health). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float MaxHealth;

	// ---------- Weapon ----------

	/** Current weapon state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	TEnumAsByte<EWeaponState> WeaponState;

	/** Is weapon currently equipped? */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	uint32 bIsWeaponEquipped : 1;

	/** Should aim over the right shoulder? False aims over left shoulder. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	uint32 bAimRightShoulder : 1;

	// ---------- Movement ----------

	/** Current movement state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	TEnumAsByte<EMovementState> MovementState;

	/** Current amount of time crouch button held down. */
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Crouch")
	float TimeCrouchBtnHeldDown;

	/** Total time to hold down crouch button to prone. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Crouch")
	float MaxTimeCrouchBtnHeldToProne;

	/** Is crouch button currently being held down? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Crouch")
	uint32 bIsCrouchBtnHeldDown : 1;

	// ---------- Camera Mode ----------

	/** Current camera mode. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	CameraMode CameraMode;

	// ---------- Smooth Crouch Camera Shift ----------

	/** Timeline curve for smooth crouch camera shift. */
	UPROPERTY(EditAnywhere, Category = "Camera|Camera Shift|Crouch")
	UCurveFloat* CrouchCameraTimelineCurve;

	/** Current timeline value for smooth crouch camera shift. */
	UPROPERTY(BlueprintReadOnly, Category = "Camera|Camera Shift|Crouch")
	float SmoothCrouchCameraShiftTimelineValue;

	// ---------- Equip Weapon Camera Shift ----------

	/** Timeline curve for weapon equip camera shift. */
	UPROPERTY(EditAnywhere, Category = "Camera|Camera Shift|Equip Weapon")
	UCurveFloat* EquipCameraShiftTimelineCurve;

	/** Current timeline value for weapon equip camera shift. */
	UPROPERTY(BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	float EquipCameraShiftTimelineValue;

	/** Camera field-of-view (FOV) when weapon is equipped. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	float IronsightCameraFOV;

	/** Camera boom target arm length when weapon is equipped. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	float IronsightBoomTargetArmLength;

	/** Camera boom socket offset when weapon is equipped. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Camera Shift|Equip Weapon")
	FVector IronsightBoomSocketOffset;

	// ---------- Wall Hug Edge Camera Shift ----------

	/**  */
	UPROPERTY(EditAnywhere, Category = "Camera|Camera Shift|Wall Hug Edge")
	UCurveFloat* WallHugEdgeCameraShiftTimelineCurve;

	/**  */
	UPROPERTY(BlueprintReadOnly, Category = "Camera|Camera Shift|Wall Hug Edge")
	float WallHugEdgeCameraShiftTimelineValue;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Camera Shift|Wall Hug Edge")
	float WallHugEdgeCameraBoomSocketOffsetY;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void MoveForward(float Value);

	/** Called for side to side input */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void MoveRight(float Value);

	void Turn(float Value);
	void LookUp(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	virtual void OnSpeedAdjust(float Value);
	virtual float CalculateMoveSpeed() const;

	/** Called for sprint input pressed */
	//void Sprint();

	/** Called for sprint input released */
	//void StopSprinting();

	/*void HandleCrouchPress();
	void HandleCrouchRelease();*/

	/** Handler for when weapon state is selected */
	void HandleWeaponStateSelect(EWeaponState SelectedWeaponState);

	void HandleWeaponSelectRifle();

	/*void AimIronsights();
	void StopAimIronsights();*/

	void CreateZeroToOneCurve(UCurveFloat* CurveFloat, FName Name, float Length = 1.f);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/** Handler for when crouch is pressed */
	UFUNCTION(BlueprintCallable, Category = "Movement|Crouch")
	void CrouchToggle();

	/*UFUNCTION(BlueprintCallable, Category = "Movement|Crouch")
	void CrouchTick(float DeltaSeconds);*/

	// ---------- Smooth Crouch Camera Shift ----------

	UFUNCTION()
	virtual void SmoothCrouchCameraShiftTimelineCallback();

	// ---------- Equip Weapon Camera Shift ----------

	UFUNCTION()
	virtual void EquipCameraShiftTimelineCallback();

	UFUNCTION(BlueprintCallable)
	virtual void PlayEquipCameraShift();

	UFUNCTION(BlueprintCallable)
	virtual void ReverseEquipCameraShift();

	// ---------- Wall Hug Edge Camera Shift ----------

	UFUNCTION()
	virtual void WallHugEdgeCameraShiftTimelineCallback();

	UFUNCTION(BlueprintCallable)
	virtual void PlayWallHugEdgeCameraShift();

	UFUNCTION(BlueprintCallable)
	virtual void ReverseWallHugEdgeCameraShift();

	UFUNCTION(BlueprintCallable)
	virtual void HandleOnWallHugChange(EOnWallHugState OldState, EOnWallHugState NewState);

	// ---------- FORCEINLINE (Private Properties) ----------

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns PostProcess subobject **/
	FORCEINLINE	class UPostProcessComponent* GetPostProcess() const { return PostProcess; }

	/** Returns VisionModeComponent subobject **/
	FORCEINLINE class UVisionModeComponent* GetVisionModeComponent() const { return VisionModeComponent; }
	/** Returns LightDetectionManager subobject **/
	FORCEINLINE class ULightDetectionManagerComponent* GetLightDetectionManager() const { return LightDetectionManager; }
	/** Returns NoiseDetectionComponent subobject **/
	FORCEINLINE class UNoiseDetectionComponent* GetNoiseDetectionComponent() const { return NoiseDetectionComponent; }
	/** Returns WallHugComponent subobject **/
	FORCEINLINE class UWallHugComponent* GetWallHugComponent() const { return WallHugComponent; }
	
	/** Returns SpeedLevel **/
	FORCEINLINE int32 GetSpeedLevel() const { return SpeedLevel; }
};

