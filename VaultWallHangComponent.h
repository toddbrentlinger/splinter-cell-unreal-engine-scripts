// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "VaultWallHangComponent.generated.h"

class ACharacter;
class UCapsuleComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum EVaultWallHangState
{
	None UMETA(DisplayName = "None"),
	VaultingFromStand UMETA(DisplayName = "Vaulting From Stand"),
	VaultingFromWallHang UMETA(DisplayName = "Vaulting From Wall Hang"),
	MovingToWallHang UMETA(DisplayName = "Moving To Wall Hang"),
	WallHanging UMETA(DisplayName = "Wall Hanging"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API UVaultWallHangComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACharacter* CharacterRef;

public:	
	// Sets default values for this component's properties
	UVaultWallHangComponent();

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	TEnumAsByte<EVaultWallHangState> VaultWallHangState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< TEnumAsByte< EObjectTypeQuery > > TraceObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceFromWallEdgeToCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VaultHeightMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WallHangHeightMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WallHangHeightMax;

	// Animation Montages

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* VaultShortWallAnimationMontage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual bool CanCapsuleFit(const FVector& Center, float Radius, float HalfHeight);

	UFUNCTION(BlueprintCallable)
	virtual bool CanVaultToHit(UCapsuleComponent* Capsule, const FHitResult& Hit, FVector& EndLocation);

	UFUNCTION(BlueprintCallable)
	virtual bool TryToVaultWallHang();

	UFUNCTION(BlueprintCallable)
	virtual void OnMontageBlendingOut(UAnimMontage* animMontage, bool bInterrupted);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void HandleMovementInput(float AxisValue, bool bIsRight);

	FORCEINLINE ACharacter* GetCharacterRef() const { return CharacterRef; }
};
