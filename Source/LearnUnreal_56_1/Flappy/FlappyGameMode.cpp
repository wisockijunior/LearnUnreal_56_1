// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "FlappyGameMode.h"
#include "FlappyBirdPawn.h"
#include "FlappyPipePair.h"
#include "FlappyHUD.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AFlappyGameMode::AFlappyGameMode()
	: PipeSpawnInterval(1.9f)
	, PipeSpawnY(900.0f)
	, MinGapZ(-140.0f)
	, MaxGapZ(180.0f)
	, GapSize(250.0f)
	, PipeSpeed(340.0f)
	, CurrentState(EFlappyGameState::Ready)
	, CurrentScore(0)
	, HighScore(0)
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = AFlappyBirdPawn::StaticClass();
	HUDClass = AFlappyHUD::StaticClass();
	PipePairClass = AFlappyPipePair::StaticClass();
}

void AFlappyGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = EFlappyGameState::Ready;
	SpawnScenery();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
	}
}

void AFlappyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlappyGameMode::StartGame()
{
	if (CurrentState != EFlappyGameState::Ready)
	{
		return;
	}

	CurrentState = EFlappyGameState::Playing;
	CurrentScore = 0;

	GetWorldTimerManager().SetTimer(TimerHandle_PipeSpawner, this, &AFlappyGameMode::SpawnPipePair, PipeSpawnInterval, true, 0.8f);
}

void AFlappyGameMode::SpawnPipePair()
{
	if (CurrentState != EFlappyGameState::Playing)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || !PipePairClass)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLoc(0.0f, PipeSpawnY, 0.0f);
	AFlappyPipePair* Pipe = World->SpawnActor<AFlappyPipePair>(PipePairClass, SpawnLoc, FRotator::ZeroRotator, Params);
	if (Pipe)
	{
		const float GapZ = FMath::RandRange(MinGapZ, MaxGapZ);
		Pipe->SetupPipes(GapZ, GapSize, PipeSpeed);
		SpawnedPipes.Add(Pipe);
	}
}

void AFlappyGameMode::OnBirdDied()
{
	CurrentState = EFlappyGameState::GameOver;
	GetWorldTimerManager().ClearTimer(TimerHandle_PipeSpawner);
	HighScore = FMath::Max(HighScore, CurrentScore);
}

void AFlappyGameMode::AddScore(int32 Amount)
{
	if (CurrentState == EFlappyGameState::Playing)
	{
		CurrentScore += Amount;
		HighScore = FMath::Max(HighScore, CurrentScore);
	}
}

void AFlappyGameMode::RestartGame()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_PipeSpawner);

	for (AActor* Pipe : SpawnedPipes)
	{
		if (IsValid(Pipe))
		{
			Pipe->Destroy();
		}
	}
	SpawnedPipes.Empty();

	CurrentScore = 0;
	CurrentState = EFlappyGameState::Ready;

	AFlappyBirdPawn* Bird = Cast<AFlappyBirdPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Bird)
	{
		Bird->ResetBird();
	}
}

void AFlappyGameMode::SpawnScenery()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UMaterialInterface* BaseMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	if (!CubeMesh || !BaseMat)
	{
		return;
	}

	// Floor visual
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, 0.0f, -390.0f), FRotator::ZeroRotator, SpawnParams);
	if (Floor && Floor->GetStaticMeshComponent())
	{
		Floor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
		Floor->SetActorScale3D(FVector(2.0f, 25.0f, 1.0f));
		UMaterialInstanceDynamic* FloorMat = UMaterialInstanceDynamic::Create(BaseMat, Floor);
		if (FloorMat)
		{
			FloorMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.25f, 0.55f, 0.20f));
			Floor->GetStaticMeshComponent()->SetMaterial(0, FloorMat);
		}
		FloorActor = Floor;
	}

	// Sky Backdrop visual
	AStaticMeshActor* Sky = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(120.0f, 0.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
	if (Sky && Sky->GetStaticMeshComponent())
	{
		Sky->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
		Sky->SetActorScale3D(FVector(0.5f, 30.0f, 16.0f));
		UMaterialInstanceDynamic* SkyMat = UMaterialInstanceDynamic::Create(BaseMat, Sky);
		if (SkyMat)
		{
			SkyMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.28f, 0.65f, 0.95f));
			Sky->GetStaticMeshComponent()->SetMaterial(0, SkyMat);
		}
		BackdropActor = Sky;
	}
}
