// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "FlappyBirdPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "FlappyGameMode.h"

AFlappyBirdPawn::AFlappyBirdPawn()
	: FlapStrength(550.0f)
	, Gravity(-1500.0f)
	, FloorZ(-340.0f)
	, CeilingZ(380.0f)
	, VerticalVelocity(0.0f)
	, bIsDead(false)
	, StartLocation(FVector(0.0f, 0.0f, 0.0f))
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Root Sphere Collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	SphereCollision->InitSphereRadius(24.0f);
	SphereCollision->SetCollisionProfileName(TEXT("Pawn"));
	SphereCollision->SetGenerateOverlapEvents(true);

	// Sphere Mesh
	BirdMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(SphereCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshFinder.Succeeded())
	{
		BirdMesh->SetStaticMesh(SphereMeshFinder.Object);
	}
	BirdMesh->SetWorldScale3D(FVector(0.48f, 0.48f, 0.48f));
	BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	BaseMaterial = MatFinder.Succeeded() ? MatFinder.Object : nullptr;

	// Camera locked looking down +X at origin
	SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCamera->SetupAttachment(SphereCollision);
	SideViewCamera->SetUsingAbsoluteLocation(true);
	SideViewCamera->SetUsingAbsoluteRotation(true);
}

void AFlappyBirdPawn::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	// Fixed side-scroller camera view
	SideViewCamera->SetWorldLocation(FVector(-850.0f, 0.0f, 0.0f));
	SideViewCamera->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));

	// Yellow bird material
	if (BaseMaterial)
	{
		UMaterialInstanceDynamic* BirdMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (BirdMat)
		{
			BirdMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.85f, 0.05f, 1.0f));
			BirdMesh->SetMaterial(0, BirdMat);
		}
	}
}

void AFlappyBirdPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Foolproof input polling for instant responsiveness
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		if (PC->WasInputKeyJustPressed(EKeys::SpaceBar) ||
			PC->WasInputKeyJustPressed(EKeys::LeftMouseButton) ||
			PC->WasInputKeyJustPressed(EKeys::Up) ||
			PC->WasInputKeyJustPressed(EKeys::W))
		{
			Flap();
		}
		else if (PC->WasInputKeyJustPressed(EKeys::R))
		{
			RequestRestart();
		}
	}

	AFlappyGameMode* GM = Cast<AFlappyGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM)
	{
		return;
	}

	if (GM->GetFlappyState() == EFlappyGameState::Playing || bIsDead)
	{
		// Apply gravity
		VerticalVelocity += Gravity * DeltaTime;
		FVector Loc = GetActorLocation();
		Loc.Z += VerticalVelocity * DeltaTime;

		// Check floor
		if (Loc.Z <= FloorZ)
		{
			Loc.Z = FloorZ;
			if (!bIsDead)
			{
				Die();
			}
		}

		// Check ceiling
		if (Loc.Z >= CeilingZ)
		{
			Loc.Z = CeilingZ;
			VerticalVelocity = 0.0f;
		}

		SetActorLocation(Loc, true);

		// Roll rotation in Y-Z plane according to velocity (tilts beak up when jumping, down when dropping)
		const float ClampedVel = FMath::Clamp(VerticalVelocity, -900.0f, 600.0f);
		const float RollAngle = -ClampedVel * 0.08f;
		BirdMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, RollAngle));
	}
}

void AFlappyBirdPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AFlappyBirdPawn::Flap);
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AFlappyBirdPawn::Flap);
		PlayerInputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AFlappyBirdPawn::Flap);
		PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AFlappyBirdPawn::Flap);
		PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AFlappyBirdPawn::RequestRestart);
	}
}

void AFlappyBirdPawn::Flap()
{
	AFlappyGameMode* GM = Cast<AFlappyGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM)
	{
		return;
	}

	if (GM->GetFlappyState() == EFlappyGameState::Ready)
	{
		GM->StartGame();
		VerticalVelocity = FlapStrength;
	}
	else if (GM->GetFlappyState() == EFlappyGameState::Playing && !bIsDead)
	{
		VerticalVelocity = FlapStrength;
	}
	else if (GM->GetFlappyState() == EFlappyGameState::GameOver)
	{
		GM->RestartGame();
	}
}

void AFlappyBirdPawn::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	VerticalVelocity = -200.0f; // slight downward bump

	if (AFlappyGameMode* GM = Cast<AFlappyGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnBirdDied();
	}
}

void AFlappyBirdPawn::RequestRestart()
{
	if (AFlappyGameMode* GM = Cast<AFlappyGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (GM->GetFlappyState() == EFlappyGameState::GameOver)
		{
			GM->RestartGame();
		}
	}
}

void AFlappyBirdPawn::ResetBird()
{
	bIsDead = false;
	VerticalVelocity = 0.0f;
	SetActorLocation(StartLocation);
	BirdMesh->SetRelativeRotation(FRotator::ZeroRotator);
}
