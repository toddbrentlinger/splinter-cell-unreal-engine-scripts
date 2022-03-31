// Fill out your copyright notice in the Description page of Project Settings.


#include "TimelineCurveVisualizer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATimelineCurveVisualizer::ATimelineCurveVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	TimelineValue = 0.f;
	HorizontalDelta = 200.f;
	VerticalDelta = 100.f;
	Duration = 1.f;

}

// Called when the game starts or when spawned
void ATimelineCurveVisualizer::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();

	if (!Curve)
	{
		Curve = NewObject<UCurveFloat>(this, TEXT("TimelineCurve"));
		Curve->FloatCurve.AddKey(0.f, 0.f);
		Curve->FloatCurve.AddKey(Duration, 1.f);

		FKeyHandle FirstKey;
		Curve->FloatCurve.AddKey(0.f, 0.f, false, FirstKey);
		Curve->FloatCurve.SetKeyInterpMode(FirstKey, ERichCurveInterpMode::RCIM_Cubic);
		Curve->FloatCurve.SetKeyTangentMode(FirstKey, ERichCurveTangentMode::RCTM_User);
		//Curve->FloatCurve.SetKeyTangentWeightMode(FirstKey, ERichCurveTangentWeightMode::RCTWM_WeightedNone);
		Curve->FloatCurve.GetKey(FirstKey).LeaveTangent = 1.f;

		FKeyHandle LastKey;
		Curve->FloatCurve.AddKey(Duration, 1.f, false, LastKey);
		Curve->FloatCurve.SetKeyInterpMode(LastKey, ERichCurveInterpMode::RCIM_Cubic);
		Curve->FloatCurve.SetKeyTangentMode(LastKey, ERichCurveTangentMode::RCTM_User);
		//Curve->FloatCurve.SetKeyTangentWeightMode(LastKey, ERichCurveTangentWeightMode::RCTWM_WeightedNone);
		Curve->FloatCurve.GetKey(LastKey).ArriveTangent = 1.f;

		Curve->FloatCurve.AutoSetTangents(); // Is this needed since no keys have been set to 'auto'
	}
	// Initialize timeline
	FOnTimelineFloat TimelineCallback;
	Timeline.SetPropertySetObject(this); // Is this needed?
	TimelineCallback.BindUFunction(this, FName(TEXT("TimelineCallback")));
	Timeline.AddInterpFloat(Curve, TimelineCallback, FName("TimelineValue"));
	
}

// Called every frame
void ATimelineCurveVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);

	if (!Timeline.IsPlaying())
		Timeline.PlayFromStart();

}

void ATimelineCurveVisualizer::TimelineCallback() 
{
	SetActorLocation(
		FVector(InitialLocation.X, InitialLocation.Y + UKismetMathLibrary::Lerp(0.f, HorizontalDelta, Timeline.GetPlaybackPosition()),
			InitialLocation.Z + UKismetMathLibrary::Lerp(0.f, VerticalDelta, TimelineValue)
		)
	);
}
