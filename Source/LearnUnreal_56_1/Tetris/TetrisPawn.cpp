// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "TetrisPawn.h"
#include "TetrisBoardActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

ATetrisPawn::ATetrisPawn()
	: CachedBoard(nullptr)
	, LastHardDropTime(0.0)
	, bCanHardDrop(true)
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
	bCanHardDrop = true;
	GetBoardActor();
}

void ATetrisPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// IMPORTANT: Do NOT poll input here (e.g. PC->WasInputKeyJustPressed(EKeys::SpaceBar)).
	// Keys are already handled by BindKey in SetupPlayerInputComponent. Polling here as well
	// caused HardDrop to run twice per press (the "two pieces dropped" bug).
	// See Docs/TETRIS_HARD_DROP_FIX.md.
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

		// Hard Drop: bind BOTH Pressed and Released so we can implement a release-gate.
		// (Unity analogy: Input.GetKeyDown + Input.GetKeyUp, but event-driven instead of polled.)
		PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ATetrisPawn::OnHardDropPressed);
		PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ATetrisPawn::OnHardDropReleased);
		PlayerInputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ATetrisPawn::OnHardDropPressed);
		PlayerInputComponent->BindKey(EKeys::Enter, IE_Released, this, &ATetrisPawn::OnHardDropReleased);

		PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &ATetrisPawn::RestartGame);
		PlayerInputComponent->BindKey(EKeys::T, IE_Pressed, this, &ATetrisPawn::TriggerAutomatedTest);
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

// =====================================================================================
// HARD DROP INPUT HANDLING -- "one Space press == exactly one piece dropped"
// Full write-up: Docs/TETRIS_HARD_DROP_FIX.md
//
// Original bug: HardDrop() fired TWICE in the same frame (BindKey callback + a second
// WasInputKeyJustPressed() poll in Tick). Call #1 locked piece A and spawned piece B,
// call #2 instantly hard-dropped piece B too -> two pieces landed, score +64 instead of +32.
//
// Protection layers:
//   Layer 1: Single input source. Tick() no longer polls keys; only BindKey drives drops.
//   Layer 2: Debounce. Reject a drop within 0.2s of the previous one (LastHardDropTime).
//   Layer 3: Release-gate. bCanHardDrop is cleared on press and only re-armed on release,
//            so OS key-repeat or holding Space can never chain extra drops.
// =====================================================================================
void ATetrisPawn::OnHardDropPressed()
{
	// Layer 3: release-gate -- ignore until the key has been physically released.
	if (!bCanHardDrop)
	{
		return;
	}
	bCanHardDrop = false;

	// Layer 2: debounce -- guards against duplicate events arriving within the same short window
	// (e.g. Space + Enter pressed together, or a duplicated binding).
	const double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastHardDropTime < 0.2)
	{
		return;
	}
	LastHardDropTime = CurrentTime;

	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->HardDrop();
	}
}

// Re-arms the release-gate (Layer 3). Bound to IE_Released for Space and Enter.
void ATetrisPawn::OnHardDropReleased()
{
	bCanHardDrop = true;
}

// Legacy entry point kept for API compatibility; routes through the guarded path.
// NOTE: not bound to any key. If called from code, the gate stays closed until a key release.
void ATetrisPawn::HardDrop()
{
	OnHardDropPressed();
}

void ATetrisPawn::RestartGame()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->RestartGame();
	}
}

void ATetrisPawn::TriggerAutomatedTest()
{
	if (ATetrisBoardActor* Board = GetBoardActor())
	{
		Board->RunAutomatedTest();
	}
}
