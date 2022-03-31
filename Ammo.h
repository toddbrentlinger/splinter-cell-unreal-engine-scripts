// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Ammo.generated.h"

UCLASS()
class SPLINTERCELL_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmo();

	/** Base point damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UDamageType> DamageType;*/

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	UFUNCTION()
	virtual bool CanApplyHitEffect(AActor* DamagedActor, AController* EventInstigator, UPARAM(ref) const FVector& HitFromDirection, UPARAM(ref) const FHitResult& HitInfo);

	UFUNCTION(BlueprintNativeEvent)
	void ApplyHitEffect(AActor* DamagedActor, AController* EventInstigator, UPARAM(ref) const FVector& HitFromDirection, UPARAM(ref) const FHitResult& HitInfo);
	virtual void ApplyHitEffect_Implementation(AActor* DamagedActor, AController* EventInstigator, UPARAM(ref) const FVector& HitFromDirection, UPARAM(ref) const FHitResult& HitInfo);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
