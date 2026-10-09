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
	, BobTimer(0.0f)
	, SquashStretchTimer(0.0f)
	, CurrentTiltAngle(0.0f)
	, WingFlapTime(0.0f)
	, DeathSpinAngle(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Root Sphere Collision (exact physical boundary)
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	SphereCollision->InitSphereRadius(24.0f);
	SphereCollision->SetCollisionProfileName(TEXT("Pawn"));
	SphereCollision->SetGenerateOverlapEvents(true);

	// BirdVisualRoot: Parent of all visual parts for unified tilt, squash, and stretch
	BirdVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BirdVisualRoot"));
	BirdVisualRoot->SetupAttachment(SphereCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
	UStaticMesh* SphereMesh = SphereMeshFinder.Succeeded() ? SphereMeshFinder.Object : nullptr;
	UStaticMesh* ConeMesh = ConeMeshFinder.Succeeded() ? ConeMeshFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	BaseMaterial = MatFinder.Succeeded() ? MatFinder.Object : nullptr;

	// 1. Body Sphere Mesh (Golden-Yellow body)
	BirdMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(BirdVisualRoot);
	if (SphereMesh)
	{
		BirdMesh->SetStaticMesh(SphereMesh);
	}
	BirdMesh->SetWorldScale3D(FVector(0.48f, 0.48f, 0.48f));
	BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2. Beak Cone Mesh (Orange beak pointing forward along +Y)
	BeakMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeakMesh"));
	BeakMesh->SetupAttachment(BirdVisualRoot);
	if (ConeMesh)
	{
		BeakMesh->SetStaticMesh(ConeMesh);
	}
	BeakMesh->SetRelativeLocation(FVector(0.0f, 22.0f, -3.0f));
	BeakMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f)); // Points tip along +Y (forward flight)
	BeakMesh->SetWorldScale3D(FVector(0.14f, 0.14f, 0.22f));
	BeakMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3. Eye White Sphere (facing camera on -X, upper front)
	EyeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EyeMesh"));
	EyeMesh->SetupAttachment(BirdVisualRoot);
	if (SphereMesh)
	{
		EyeMesh->SetStaticMesh(SphereMesh);
	}
	EyeMesh->SetRelativeLocation(FVector(-14.0f, 10.0f, 8.0f));
	EyeMesh->SetWorldScale3D(FVector(0.18f, 0.18f, 0.18f));
	EyeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 4. Pupil Black Sphere
	PupilMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PupilMesh"));
	PupilMesh->SetupAttachment(BirdVisualRoot);
	if (SphereMesh)
	{
		PupilMesh->SetStaticMesh(SphereMesh);
	}
	PupilMesh->SetRelativeLocation(FVector(-19.0f, 13.0f, 8.0f));
	PupilMesh->SetWorldScale3D(FVector(0.09f, 0.09f, 0.09f));
	PupilMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 5. Wing Mesh (Flattened ellipsoid on side facing camera)
	WingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WingMesh"));
	WingMesh->SetupAttachment(BirdVisualRoot);
	if (SphereMesh)
	{
		WingMesh->SetStaticMesh(SphereMesh);
	}
	WingMesh->SetRelativeLocation(FVector(-16.0f, -5.0f, -2.0f));
	WingMesh->SetWorldScale3D(FVector(0.10f, 0.26f, 0.18f));
	WingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	if (BaseMaterial)
	{
		// 1. Body: Warm Golden-Yellow
		UMaterialInstanceDynamic* BodyMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (BodyMat)
		{
			BodyMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.85f, 0.05f, 1.0f));
			BirdMesh->SetMaterial(0, BodyMat);
		}

		// 2. Beak: Vibrant Orange
		UMaterialInstanceDynamic* BeakMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (BeakMat)
		{
			BeakMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.45f, 0.0f, 1.0f));
			BeakMesh->SetMaterial(0, BeakMat);
		}

		// 3. Eye: Clean White
		UMaterialInstanceDynamic* EyeMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (EyeMat)
		{
			EyeMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			EyeMesh->SetMaterial(0, EyeMat);
		}

		// 4. Pupil: Deep Black
		UMaterialInstanceDynamic* PupilMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (PupilMat)
		{
			PupilMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.02f, 0.02f, 0.02f, 1.0f));
			PupilMesh->SetMaterial(0, PupilMat);
		}

		// 5. Wing: Cream / Light Feather
		UMaterialInstanceDynamic* WingMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (WingMat)
		{
			WingMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.96f, 0.72f, 1.0f));
			WingMesh->SetMaterial(0, WingMat);
		}
	}
}

void AFlappyBirdPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Direct input polling for responsive jumps and restarts
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

	const EFlappyGameState State = GM->GetFlappyState();

	// =========================================================================
	// STATE 1: READY (Idle hover bobbing & gentle wing breathing)
	// =========================================================================
	if (State == EFlappyGameState::Ready)
	{
		BobTimer += DeltaTime;
		const float BobZ = FMath::Sin(BobTimer * 5.0f) * 14.0f;
		FVector IdleLoc = StartLocation;
		IdleLoc.Z += BobZ;
		SetActorLocation(IdleLoc);

		// Smoothly restore upright posture and scale
		CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, 0.0f, DeltaTime, 8.0f);
		BirdVisualRoot->SetRelativeRotation(FRotator(0.0f, 0.0f, CurrentTiltAngle));
		BirdVisualRoot->SetRelativeScale3D(FVector::OneVector);

		// Gentle resting wing breathing
		const float WingBob = FMath::Sin(BobTimer * 7.0f) * 10.0f;
		WingMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, WingBob));
		return;
	}

	// =========================================================================
	// STATE 2: PLAYING or DEAD (Physics, falling, dynamic tilt, wing flap, squash/stretch)
	// =========================================================================
	if (State == EFlappyGameState::Playing || bIsDead)
	{
		// 1. Gravity and Vertical Movement
		VerticalVelocity += Gravity * DeltaTime;
		FVector Loc = GetActorLocation();
		Loc.Z += VerticalVelocity * DeltaTime;

		// Floor collision
		if (Loc.Z <= FloorZ)
		{
			Loc.Z = FloorZ;
			if (!bIsDead)
			{
				Die();
			}
		}

		// Ceiling collision
		if (Loc.Z >= CeilingZ)
		{
			Loc.Z = CeilingZ;
			VerticalVelocity = 0.0f;
		}

		SetActorLocation(Loc, true);

		// 2. Pitch / Tilt Rotation in Y-Z plane
		if (bIsDead)
		{
			// Tumble spin when dead until resting on floor
			if (Loc.Z > FloorZ)
			{
				DeathSpinAngle += 650.0f * DeltaTime;
				BirdVisualRoot->SetRelativeRotation(FRotator(0.0f, 0.0f, DeathSpinAngle));
			}
			else
			{
				// Settle nose-down on the floor
				BirdVisualRoot->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
			}
		}
		else
		{
			// Dynamic tilt physics:
			// Ascending (Velocity > 0): snaps upward (-28 deg).
			// Descending (Velocity < 0): progressively dives downward (+75 deg).
			float TargetRoll = 0.0f;
			if (VerticalVelocity > 50.0f)
			{
				TargetRoll = -28.0f; // Beak points up
			}
			else
			{
				const float DropRatio = FMath::Clamp(-VerticalVelocity / 800.0f, 0.0f, 1.0f);
				TargetRoll = FMath::Lerp(0.0f, 75.0f, DropRatio);
			}

			const float InterpSpeed = (VerticalVelocity > 0.0f) ? 14.0f : 5.0f;
			CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, TargetRoll, DeltaTime, InterpSpeed);
			BirdVisualRoot->SetRelativeRotation(FRotator(0.0f, 0.0f, CurrentTiltAngle));
		}

		// 3. Squash and Stretch on Jump
		if (SquashStretchTimer > 0.0f)
		{
			SquashStretchTimer -= DeltaTime;
			const float Alpha = FMath::Clamp(SquashStretchTimer / 0.18f, 0.0f, 1.0f);
			// Stretch along Z (vertical jump), squash along Y
			const float ScaleZ = 1.0f + 0.22f * Alpha;
			const float ScaleY = 1.0f - 0.16f * Alpha;
			BirdVisualRoot->SetRelativeScale3D(FVector(1.0f, ScaleY, ScaleZ));
		}
		else
		{
			BirdVisualRoot->SetRelativeScale3D(FVector::OneVector);
		}

		// 4. Wing Flap Animation
		if (WingFlapTime > 0.0f)
		{
			WingFlapTime -= DeltaTime;
			const float FlapProgress = 1.0f - (WingFlapTime / 0.22f);
			const float FlapAngle = FMath::Sin(FlapProgress * PI * 2.0f) * 35.0f;
			WingMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, FlapAngle));
		}
		else
		{
			WingMesh->SetRelativeRotation(FRotator::ZeroRotator);
		}
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
		SquashStretchTimer = 0.18f;
		WingFlapTime = 0.22f;
	}
	else if (GM->GetFlappyState() == EFlappyGameState::Playing && !bIsDead)
	{
		VerticalVelocity = FlapStrength;
		SquashStretchTimer = 0.18f;
		WingFlapTime = 0.22f;
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
	VerticalVelocity = 250.0f; // Classic death hop up before falling
	DeathSpinAngle = CurrentTiltAngle;

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
	BobTimer = 0.0f;
	SquashStretchTimer = 0.0f;
	CurrentTiltAngle = 0.0f;
	WingFlapTime = 0.0f;
	DeathSpinAngle = 0.0f;
	SetActorLocation(StartLocation);
	BirdVisualRoot->SetRelativeRotation(FRotator::ZeroRotator);
	BirdVisualRoot->SetRelativeScale3D(FVector::OneVector);
	WingMesh->SetRelativeRotation(FRotator::ZeroRotator);
}
