// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "TetrisPawn.h"
#include "TetrisBoardActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

ATetrisPawn::ATetrisPawn()
	: CachedBoard(nullptr)
	, LastHardDropTime(0.0)
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	OrthoCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OrthoCamera"));
	OrthoCamera->SetupAttachment(SceneRoot);
	OrthoCamera->SetRelativeLocation(FVector(-850.0f, 0.0f, 380.0f));
	OrthoCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void ATetrisPawn::BeginPlay()
{
	Super::BeginPlay();
	GetBoardActor();
}

void ATetrisPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATetrisPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &ATetrisPawn::MoveLeft);
		PlayerInputComponent->BindKey(EKeys::Left, IE_Pressed, this, &ATetrisPawn::MoveLeft);

		PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &ATetrisPawn::MoveRight);
		PlayerInputComponent->BindKey(EKeys::Right, IE_Pressed, this, &ATetrisPawn::MoveRight);

		PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &ATetrisPawn::RotatePiece);
		PlayerInputComponent->BindKey(EKeys::Up, IE_Pressed, this, &ATetrisPawn::RotatePiece);

		PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &ATetrisPawn::SoftDrop);
		PlayerInputComponent->BindKey(EKeys::Down, IE_Pressed, this, &ATetrisPawn::SoftDrop);

		PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ATetrisPawn::HardDrop);
		PlayerInputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ATetrisPawn::HardDrop);

		PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &ATetrisPawn::RestartGame);
	}
}

ATetrisBoardActor* ATetrisPawn::GetBoardActor()
{
	if (!CachedBoard)
	{
		CachedBoard = Cast<ATetrisBoardActor>(UGameplayStatics::GetActorOfClass(this, ATetrisBoardActor::StaticClass()));
	}
	return CachedBoard;
}

void ATetrisPawn::MoveLeft()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->MoveLeft();
	}
}

void ATetrisPawn::MoveRight()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->MoveRight();
	}
}

void ATetrisPawn::RotatePiece()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->RotatePiece();
	}
}

void ATetrisPawn::SoftDrop()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->SoftDrop();
	}
}

void ATetrisPawn::HardDrop()
{
	const double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastHardDropTime < 0.15)
	{
		return;
	}
	LastHardDropTime = CurrentTime;

	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->HardDrop();
	}
}

void ATetrisPawn::RestartGame()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->RestartGame();
	}
}
