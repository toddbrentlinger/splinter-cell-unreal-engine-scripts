// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallHugComponent.generated.h"

class ACharacter;

UENUM(BlueprintType)
enum class EWallHugState : uint8
{
	None UMETA(DisplayName = "None"),
	MovingToWall UMETA(DisplayName = "Moving To Wall"),
	OnWall UMETA(DisplayName = "On Wall"),
	MovingFromWall UMETA(DisplayName = "Moving From Wall"),
};

UENUM(BlueprintType)
enum class EOnWallHugState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Moving UMETA(DisplayName = "Moving"),
	Edge UMETA(DisplayName = "Edge"),
	Turning180 UMETA(DisplayName = "Turning 180"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeOnWallHugDelegate, EOnWallHugState, OldState, EOnWallHugState, NewState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API UWallHugComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACharacter* CharacterRef;

	//UAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWallHugState WallHugState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EOnWallHugState OnWallHugState;

	// Move To Wall

	UPROPERTY()
	FVector MoveToWallBegin;

	UPROPERTY()
	FVector MoveToWallEnd;

	UPROPERTY()
	float MoveToWallAlpha;

	UPROPERTY()
	FRotator MoveToWallRotBegin;

	UPROPERTY()
	FRotator MoveToWallRotEnd;

	// On Wall

	UPROPERTY()
	FVector WallNormal;

	// Turn 180

	//UPROPERTY()
	//FTimerHandle Turn180TimerHandle;

	UPROPERTY()
	FRotator BaseMeshRotation;

public:	
	// Sets default values for this component's properties
	UWallHugComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint32 bIsFacingRight : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MoveToWall)
	float MoveToWallDuration;

	/* Distance from edge of capsule to check for edge/corner of wall to initiate Edge state. */
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EdgeCheckDistanceFromCapsule;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> WallHugTraceChannel;

	// Turn 180

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* Turn180LeftToRightStandingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FRotator Turn180LeftToRightStandingRotatorOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* Turn180RightToLeftStandingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FRotator Turn180RightToLeftStandingRotatorOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* Turn180LeftToRightCrouchingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FRotator Turn180LeftToRightCrouchingRotatorOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* Turn180RightToLeftCrouchingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FRotator Turn180RightToLeftCrouchingRotatorOffset;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual bool FindClosestWall(const FVector& LineTraceStart, const int NumLineTraces, FVector& WallLocation, FVector& WallNormal);

	virtual bool LineTrace(const FVector& Start, float RotationAngleFromForward, FHitResult& OutHit);

	virtual void TickMovingToWall(float DeltaTime);

	virtual void TickMovingFromWall(float DeltaTime);

	virtual void TickOnWall(float DeltaTime);

	virtual void HandleMovementInputOnWall(float AxisValue, bool bIsRight);

	virtual void SetOnWallHugState(const EOnWallHugState NewOnWallHugState);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual bool TryBeginWallHug();

	UFUNCTION(BlueprintCallable)
	virtual void EndWallHug();

	UFUNCTION(BlueprintCallable)
	virtual void OnTurn180Finished(UAnimMontage* AnimMontage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	virtual bool PlayTurn180Montage(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage, FRotator& RotatorOffset);

	UFUNCTION(BlueprintCallable)
	virtual void HandleMovementInput(float AxisValue, bool bIsRight);

	UPROPERTY(BlueprintAssignable)
	FOnChangeOnWallHugDelegate OnChangeOnWallHug;

	/*UFUNCTION(BlueprintNativeEvent)
	void OnChangeOnWallHug(EOnWallHugState PrevState, EOnWallHugState NewState);
	virtual void OnChangeOnWallHug_Implementation(EOnWallHugState PrevState, EOnWallHugState NewState);*/

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="OnStartWallHugEdge"))
	void OnStartWallHugEdge();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStopWallHugEdge"))
	void OnStopWallHugEdge();

	// FORCEINLINES

	FORCEINLINE ACharacter* GetCharacterRef() const { return CharacterRef; }
	//FORCEINLINE UAnimInstance* GetAnimInstance() const { return AnimInstance; }
	FORCEINLINE EWallHugState GetWallHugState() const { return WallHugState; }
	FORCEINLINE EOnWallHugState GetOnWallHugState() const { return OnWallHugState; }
};
