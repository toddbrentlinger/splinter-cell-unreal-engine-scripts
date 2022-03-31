// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VisionModeComponent.generated.h"

class UPostProcessComponent;

UENUM(BlueprintType)
enum class EVisionMode : uint8
{
	Default UMETA(DisplayName = "Default"),
	Night UMETA(DisplayName = "Night"),
	Thermal UMETA(DisplayName = "Thermal"),
	EMF UMETA(DisplayName = "EMF"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLINTERCELL_API UVisionModeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Post Process", meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessComponentRef;

	/** Currently selected vision mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Post Process", meta = (AllowPrivateAccess = "true"))
	EVisionMode VisionMode;

public:	
	// Sets default values for this component's properties
	UVisionModeComponent();

	/** Default post process settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Post Process")
	FPostProcessSettings DefaultPostProcess;

	/** Night vision post process settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Post Process")
	FPostProcessSettings NightVisionPostProcess;

	/** Thermal vision post process settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Post Process")
	FPostProcessSettings ThermalVisionPostProcess;

	/** EMF vision post process settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Post Process")
	FPostProcessSettings EMFVisionPostProcess;

	/** Sound for vision mode turning on from default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* TurnOnVisionModeSound;

	/** Sound for changing vision modes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* ChangeVisionModeSound;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void SelectVisionMode(EVisionMode SelectedVisionMode);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE EVisionMode GetVisionMode() const { return VisionMode; }
};
