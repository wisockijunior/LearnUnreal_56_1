// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "FlappyPipePair.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "FlappyGameMode.h"
#include "FlappyBirdPawn.h"

AFlappyPipePair::AFlappyPipePair()
	: MoveSpeed(320.0f)
	, PipeLength(800.0f)
	, PipeDiameter(100.0f)
	, DestroyYThreshold(-1200.0f)
	, bScoreGiven(false)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	UStaticMesh* CylinderMesh = CylinderMeshFinder.Succeeded() ? CylinderMeshFinder.Object : nullptr;

	// Top pipe
	TopPipeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopPipeMesh"));
	TopPipeMesh->SetupAttachment(SceneRoot);
	if (CylinderMesh)
	{
		TopPipeMesh->SetStaticMesh(CylinderMesh);
	}
	TopPipeMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	TopPipeMesh->SetGenerateOverlapEvents(true);

	// Bottom pipe
	BottomPipeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomPipeMesh"));
	BottomPipeMesh->SetupAttachment(SceneRoot);
	if (CylinderMesh)
	{
		BottomPipeMesh->SetStaticMesh(CylinderMesh);
	}
	BottomPipeMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BottomPipeMesh->SetGenerateOverlapEvents(true);

	// Score Trigger box between pipes
	ScoreTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ScoreTrigger"));
	ScoreTrigger->SetupAttachment(SceneRoot);
	ScoreTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ScoreTrigger->SetGenerateOverlapEvents(true);
	ScoreTrigger->SetBoxExtent(FVector(50.0f, 20.0f, 120.0f));
}

void AFlappyPipePair::BeginPlay()
{
	Super::BeginPlay();

	ScoreTrigger->OnComponentBeginOverlap.AddDynamic(this, &AFlappyPipePair::OnScoreTriggerOverlap);
	TopPipeMesh->OnComponentHit.AddDynamic(this, &AFlappyPipePair::OnPipeHit);
	BottomPipeMesh->OnComponentHit.AddDynamic(this, &AFlappyPipePair::OnPipeHit);

	// Apply green pipe material
	UMaterialInterface* BaseMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMat)
	{
		UMaterialInstanceDynamic* PipeMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		if (PipeMat)
		{
			PipeMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 0.75f, 0.18f, 1.0f));
			TopPipeMesh->SetMaterial(0, PipeMat);
			BottomPipeMesh->SetMaterial(0, PipeMat);
		}
	}
}

void AFlappyPipePair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move left along Y axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Y -= MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation);

	// Destroy when off-screen
	if (NewLocation.Y < DestroyYThreshold)
	{
		Destroy();
	}
}

void AFlappyPipePair::SetupPipes(float InGapCenterZ, float InGapSize, float InSpeed)
{
	MoveSpeed = InSpeed;

	// In Engine/BasicShapes/Cylinder, default height is 100 cm (from -50 to +50 cm), diameter is 100 cm.
	// We scale Z so height is PipeLength (e.g. 800 cm -> Scale.Z = 8.0).
	// Scale X and Y to PipeDiameter / 100.
	const float ScaleZ = PipeLength / 100.0f;
	const float ScaleXY = PipeDiameter / 100.0f;
	const FVector PipeScale(ScaleXY, ScaleXY, ScaleZ);

	TopPipeMesh->SetWorldScale3D(PipeScale);
	BottomPipeMesh->SetWorldScale3D(PipeScale);

	// Gap bounds:
	const float HalfGap = InGapSize * 0.5f;
	const float HalfPipe = PipeLength * 0.5f;

	// Pivot is at center:
	// Top pipe bottom edge is at InGapCenterZ + HalfGap
	// So Top pipe center is at InGapCenterZ + HalfGap + HalfPipe
	TopPipeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, InGapCenterZ + HalfGap + HalfPipe));

	// Bottom pipe top edge is at InGapCenterZ - HalfGap
	// So Bottom pipe center is at InGapCenterZ - HalfGap - HalfPipe
	BottomPipeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, InGapCenterZ - HalfGap - HalfPipe));

	// Score Trigger centered in gap
	ScoreTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, InGapCenterZ));
	ScoreTrigger->SetBoxExtent(FVector(50.0f, 20.0f, HalfGap));
}

// =====================================================================================
// SCORE TRIGGER OVERLAP HANDLER
// 
// Unity Analogy:
//   void OnTriggerEnter(Collider other) {
//       if (bScoreGiven) return;
//       FlappyBird bird = other.GetComponent<FlappyBird>();
//       if (bird != null && !bird.IsDead) {
//           bScoreGiven = true;
//           GameManager.Instance.AddScore(1);
//       }
//   }
//
// How Unreal handles it:
// 1. Bound to ScoreTrigger->OnComponentBeginOverlap in BeginPlay().
// 2. Fires when an actor (OtherActor) enters the invisible box volume positioned in the gap.
// 3. Guards against multiple scoring via bScoreGiven flag (idempotent).
// 4. Validates the intruder is specifically the player's bird (Cast<AFlappyBirdPawn>).
// 5. Ensures the bird is alive (no points awarded if bird crashed and tumbled through).
// 6. Calls the level's GameMode (AFlappyGameMode) to increment score and update HUD.
// =====================================================================================
void AFlappyPipePair::OnScoreTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Step 1: Idempotency check -- each pipe obstacle must award points exactly once.
	if (bScoreGiven)
	{
		return;
	}

	// Step 2: Type verification -- ensure the overlapping actor is the player's Bird
	// (Cast<T> is Unreal's safe dynamic cast; returns nullptr if OtherActor is not AFlappyBirdPawn).
	AFlappyBirdPawn* Bird = Cast<AFlappyBirdPawn>(OtherActor);
	if (Bird && !Bird->IsDead())
	{
		// Step 3: Mark this pipe as scored so subsequent frames or multiple overlapping components won't re-trigger
		bScoreGiven = true;

		// Step 4: Locate the active GameMode and notify it to add 1 point
		if (AFlappyGameMode* GM = Cast<AFlappyGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->AddScore(1);
		}
	}
}

void AFlappyPipePair::OnPipeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AFlappyBirdPawn* Bird = Cast<AFlappyBirdPawn>(OtherActor);
	if (Bird && !Bird->IsDead())
	{
		Bird->Die();
	}
}
