// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "TetrisBoardActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	struct FIntPointOffset
	{
		int32 dCol;
		int32 dRow;
	};

	static const FIntPointOffset PIECE_SHAPES[8][4][4] = {
		// 0: None
		{
			{{0,0},{0,0},{0,0},{0,0}},
			{{0,0},{0,0},{0,0},{0,0}},
			{{0,0},{0,0},{0,0},{0,0}},
			{{0,0},{0,0},{0,0},{0,0}}
		},
		// 1: I
		{
			{{-1,0},{0,0},{1,0},{2,0}},
			{{1,-1},{1,0},{1,1},{1,2}},
			{{-1,1},{0,1},{1,1},{2,1}},
			{{0,-1},{0,0},{0,1},{0,2}}
		},
		// 2: O
		{
			{{0,0},{1,0},{0,1},{1,1}},
			{{0,0},{1,0},{0,1},{1,1}},
			{{0,0},{1,0},{0,1},{1,1}},
			{{0,0},{1,0},{0,1},{1,1}}
		},
		// 3: T
		{
			{{-1,0},{0,0},{1,0},{0,1}},
			{{0,-1},{0,0},{0,1},{1,0}},
			{{-1,0},{0,0},{1,0},{0,-1}},
			{{0,-1},{0,0},{0,1},{-1,0}}
		},
		// 4: S
		{
			{{-1,0},{0,0},{0,1},{1,1}},
			{{0,1},{0,0},{1,0},{1,-1}},
			{{-1,-1},{0,-1},{0,0},{1,0}},
			{{-1,1},{-1,0},{0,0},{0,-1}}
		},
		// 5: Z
		{
			{{-1,1},{0,1},{0,0},{1,0}},
			{{1,1},{1,0},{0,0},{0,-1}},
			{{-1,0},{0,0},{0,-1},{1,-1}},
			{{0,1},{0,0},{-1,0},{-1,-1}}
		},
		// 6: J
		{
			{{-1,1},{-1,0},{0,0},{1,0}},
			{{1,1},{0,1},{0,0},{0,-1}},
			{{-1,0},{0,0},{1,0},{1,-1}},
			{{0,1},{0,0},{0,-1},{-1,-1}}
		},
		// 7: L
		{
			{{-1,0},{0,0},{1,0},{1,1}},
			{{0,1},{0,0},{0,-1},{1,-1}},
			{{-1,-1},{-1,0},{0,0},{1,0}},
			{{-1,1},{0,1},{0,0},{0,-1}}
		}
	};
}

ATetrisBoardActor::ATetrisBoardActor()
	: BaseDropInterval(0.75f)
	, CurrentPieceType(1)
	, CurrentRotation(0)
	, CurrentCol(4)
	, CurrentRow(18)
	, NextPieceType(2)
	, DropTimer(0.0f)
	, CurrentDropInterval(0.75f)
	, Score(0)
	, LinesCleared(0)
	, Level(1)
	, bGameOver(false)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	CubeMesh = CubeFinder.Succeeded() ? CubeFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	BaseMaterial = MatFinder.Succeeded() ? MatFinder.Object : nullptr;

	// Active piece blocks
	for (int32 i = 0; i < 4; ++i)
	{
		FString CompName = FString::Printf(TEXT("ActiveBlock_%d"), i);
		UStaticMeshComponent* Block = CreateDefaultSubobject<UStaticMeshComponent>(*CompName);
		Block->SetupAttachment(SceneRoot);
		if (CubeMesh)
		{
			Block->SetStaticMesh(CubeMesh);
		}
		Block->SetWorldScale3D(FVector(0.36f, 0.36f, 0.36f));
		Block->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ActivePieceBlocks.Add(Block);
	}

	// Next piece preview blocks
	for (int32 i = 0; i < 4; ++i)
	{
		FString CompName = FString::Printf(TEXT("NextBlock_%d"), i);
		UStaticMeshComponent* Block = CreateDefaultSubobject<UStaticMeshComponent>(*CompName);
		Block->SetupAttachment(SceneRoot);
		if (CubeMesh)
		{
			Block->SetStaticMesh(CubeMesh);
		}
		Block->SetWorldScale3D(FVector(0.32f, 0.32f, 0.32f));
		Block->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NextPieceBlocks.Add(Block);
	}

	// ISMs for locked blocks (indices 0..7)
	for (int32 i = 0; i <= 7; ++i)
	{
		FString ISMName = FString::Printf(TEXT("PieceISM_%d"), i);
		UInstancedStaticMeshComponent* ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(*ISMName);
		ISM->SetupAttachment(SceneRoot);
		if (CubeMesh)
		{
			ISM->SetStaticMesh(CubeMesh);
		}
		ISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PieceISMs.Add(ISM);
	}
}

void ATetrisBoardActor::BeginPlay()
{
	Super::BeginPlay();

	InitBoardVisuals();
	CreateFrame();

	FMath::RandInit(FPlatformTime::Cycles());
	NextPieceType = FMath::RandRange(1, 7);

	RestartGame();
}

void ATetrisBoardActor::InitBoardVisuals()
{
	if (!BaseMaterial)
	{
		return;
	}

	// Assign materials to ISMs and cache them
	PieceMaterials.SetNum(8);
	for (int32 i = 1; i <= 7; ++i)
	{
		UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(TEXT("Color"), GetPieceColor(i));
			PieceMaterials[i] = DynMat;
			PieceISMs[i]->SetMaterial(0, DynMat);
		}
	}
}

void ATetrisBoardActor::CreateFrame()
{
	if (!CubeMesh || !BaseMaterial)
	{
		return;
	}

	UMaterialInstanceDynamic* FrameMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	if (FrameMat)
	{
		FrameMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.2f, 0.2f, 0.25f));
	}

	UMaterialInstanceDynamic* BgMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	if (BgMat)
	{
		BgMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.04f, 0.04f, 0.06f));
	}

	// Left border
	UStaticMeshComponent* LeftBorder = NewObject<UStaticMeshComponent>(this, TEXT("LeftBorder"));
	LeftBorder->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
	LeftBorder->SetStaticMesh(CubeMesh);
	LeftBorder->SetMaterial(0, FrameMat);
	LeftBorder->SetRelativeLocation(FVector(0.0f, -200.0f, 380.0f));
	LeftBorder->SetWorldScale3D(FVector(0.4f, 0.2f, 7.8f));
	LeftBorder->RegisterComponent();

	// Right border
	UStaticMeshComponent* RightBorder = NewObject<UStaticMeshComponent>(this, TEXT("RightBorder"));
	RightBorder->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
	RightBorder->SetStaticMesh(CubeMesh);
	RightBorder->SetMaterial(0, FrameMat);
	RightBorder->SetRelativeLocation(FVector(0.0f, 200.0f, 380.0f));
	RightBorder->SetWorldScale3D(FVector(0.4f, 0.2f, 7.8f));
	RightBorder->RegisterComponent();

	// Bottom border
	UStaticMeshComponent* BottomBorder = NewObject<UStaticMeshComponent>(this, TEXT("BottomBorder"));
	BottomBorder->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
	BottomBorder->SetStaticMesh(CubeMesh);
	BottomBorder->SetMaterial(0, FrameMat);
	BottomBorder->SetRelativeLocation(FVector(0.0f, 0.0f, -10.0f));
	BottomBorder->SetWorldScale3D(FVector(0.4f, 4.2f, 0.2f));
	BottomBorder->RegisterComponent();

	// Dark backplane
	UStaticMeshComponent* Backplane = NewObject<UStaticMeshComponent>(this, TEXT("Backplane"));
	Backplane->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
	Backplane->SetStaticMesh(CubeMesh);
	Backplane->SetMaterial(0, BgMat);
	Backplane->SetRelativeLocation(FVector(20.0f, 0.0f, 380.0f));
	Backplane->SetWorldScale3D(FVector(0.1f, 3.8f, 7.6f));
	Backplane->RegisterComponent();
}

void ATetrisBoardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGameOver)
	{
		return;
	}

	DropTimer += DeltaTime;
	if (DropTimer >= CurrentDropInterval)
	{
		DropTimer = 0.0f;
		if (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol, CurrentRow - 1))
		{
			CurrentRow--;
			UpdateActivePieceVisuals();
		}
		else
		{
			LockPiece();
		}
	}
}

void ATetrisBoardActor::RestartGame()
{
	FMemory::Memzero(Grid, sizeof(Grid));

	Score = 0;
	LinesCleared = 0;
	Level = 1;
	bGameOver = false;
	CurrentDropInterval = BaseDropInterval;
	DropTimer = 0.0f;

	RebuildGridVisuals();
	SpawnNewPiece();
}

void ATetrisBoardActor::SpawnNewPiece()
{
	CurrentPieceType = NextPieceType;
	NextPieceType = FMath::RandRange(1, 7);

	CurrentRotation = 0;
	CurrentCol = 4;
	CurrentRow = 18;

	// Check if spawn position is blocked -> Game Over
	if (!IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol, CurrentRow))
	{
		bGameOver = true;
		for (UStaticMeshComponent* Block : ActivePieceBlocks)
		{
			Block->SetVisibility(false);
		}
		return;
	}

	UpdateActivePieceVisuals();
	UpdateNextPieceVisuals();
}

bool ATetrisBoardActor::IsValidPosition(int32 PieceType, int32 Rotation, int32 Col, int32 Row) const
{
	for (int32 i = 0; i < 4; ++i)
	{
		const int32 TargetCol = Col + PIECE_SHAPES[PieceType][Rotation][i].dCol;
		const int32 TargetRow = Row + PIECE_SHAPES[PieceType][Rotation][i].dRow;

		// Boundary check
		if (TargetCol < 0 || TargetCol >= GRID_COLS || TargetRow < 0)
		{
			return false;
		}

		// Existing locked block check
		if (TargetRow < GRID_ROWS && Grid[TargetRow][TargetCol] != 0)
		{
			return false;
		}
	}
	return true;
}

void ATetrisBoardActor::MoveLeft()
{
	if (bGameOver) return;

	if (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol - 1, CurrentRow))
	{
		CurrentCol--;
		UpdateActivePieceVisuals();
	}
}

void ATetrisBoardActor::MoveRight()
{
	if (bGameOver) return;

	if (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol + 1, CurrentRow))
	{
		CurrentCol++;
		UpdateActivePieceVisuals();
	}
}

void ATetrisBoardActor::RotatePiece()
{
	if (bGameOver) return;

	const int32 NewRot = (CurrentRotation + 1) % 4;

	// Basic rotation & simple wall kick test
	if (IsValidPosition(CurrentPieceType, NewRot, CurrentCol, CurrentRow))
	{
		CurrentRotation = NewRot;
		UpdateActivePieceVisuals();
	}
	else if (IsValidPosition(CurrentPieceType, NewRot, CurrentCol - 1, CurrentRow))
	{
		CurrentCol -= 1;
		CurrentRotation = NewRot;
		UpdateActivePieceVisuals();
	}
	else if (IsValidPosition(CurrentPieceType, NewRot, CurrentCol + 1, CurrentRow))
	{
		CurrentCol += 1;
		CurrentRotation = NewRot;
		UpdateActivePieceVisuals();
	}
}

void ATetrisBoardActor::SoftDrop()
{
	if (bGameOver) return;

	if (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol, CurrentRow - 1))
	{
		CurrentRow--;
		Score += 1;
		DropTimer = 0.0f;
		UpdateActivePieceVisuals();
	}
	else
	{
		LockPiece();
	}
}

void ATetrisBoardActor::HardDrop()
{
	if (bGameOver) return;

	int32 DropDistance = 0;
	while (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol, CurrentRow - 1))
	{
		CurrentRow--;
		DropDistance++;
	}

	Score += DropDistance * 2;
	DropTimer = 0.0f;
	LockPiece();
}

void ATetrisBoardActor::LockPiece()
{
	for (int32 i = 0; i < 4; ++i)
	{
		const int32 C = CurrentCol + PIECE_SHAPES[CurrentPieceType][CurrentRotation][i].dCol;
		const int32 R = CurrentRow + PIECE_SHAPES[CurrentPieceType][CurrentRotation][i].dRow;

		if (R >= 0 && R < GRID_ROWS && C >= 0 && C < GRID_COLS)
		{
			Grid[R][C] = CurrentPieceType;
		}
	}

	ClearLines();
	RebuildGridVisuals();
	SpawnNewPiece();
}

void ATetrisBoardActor::ClearLines()
{
	int32 ClearedCount = 0;

	for (int32 r = 0; r < GRID_ROWS; ++r)
	{
		bool bRowFull = true;
		for (int32 c = 0; c < GRID_COLS; ++c)
		{
			if (Grid[r][c] == 0)
			{
				bRowFull = false;
				break;
			}
		}

		if (bRowFull)
		{
			ClearedCount++;

			// Shift rows down
			for (int32 shiftRow = r; shiftRow < GRID_ROWS - 1; ++shiftRow)
			{
				for (int32 c = 0; c < GRID_COLS; ++c)
				{
					Grid[shiftRow][c] = Grid[shiftRow + 1][c];
				}
			}

			// Clear top row
			for (int32 c = 0; c < GRID_COLS; ++c)
			{
				Grid[GRID_ROWS - 1][c] = 0;
			}

			r--; // recheck same row index since new blocks shifted into it
		}
	}

	if (ClearedCount > 0)
	{
		LinesCleared += ClearedCount;

		// Standard scoring: 1 = 100, 2 = 300, 3 = 500, 4 = 800
		static const int32 LineScores[] = { 0, 100, 300, 500, 800 };
		const int32 ClampedCount = FMath::Clamp(ClearedCount, 1, 4);
		Score += LineScores[ClampedCount] * Level;

		// Level up every 10 lines
		Level = 1 + (LinesCleared / 10);
		CurrentDropInterval = FMath::Max(0.08f, BaseDropInterval - (Level - 1) * 0.06f);
	}
}

void ATetrisBoardActor::UpdateActivePieceVisuals()
{
	UMaterialInstanceDynamic* PieceMat = (CurrentPieceType >= 1 && CurrentPieceType <= 7 && PieceMaterials.IsValidIndex(CurrentPieceType))
		? PieceMaterials[CurrentPieceType]
		: nullptr;

	for (int32 i = 0; i < 4; ++i)
	{
		const int32 C = CurrentCol + PIECE_SHAPES[CurrentPieceType][CurrentRotation][i].dCol;
		const int32 R = CurrentRow + PIECE_SHAPES[CurrentPieceType][CurrentRotation][i].dRow;

		ActivePieceBlocks[i]->SetVisibility(true);
		ActivePieceBlocks[i]->SetRelativeLocation(GridToLocalLocation(C, R));
		if (PieceMat)
		{
			ActivePieceBlocks[i]->SetMaterial(0, PieceMat);
		}
	}
}

void ATetrisBoardActor::UpdateNextPieceVisuals()
{
	UMaterialInstanceDynamic* NextMat = (NextPieceType >= 1 && NextPieceType <= 7 && PieceMaterials.IsValidIndex(NextPieceType))
		? PieceMaterials[NextPieceType]
		: nullptr;

	// Preview placed at Y = 280, Z = 600 in local coordinates
	const FVector PreviewCenter(0.0f, 280.0f, 600.0f);

	for (int32 i = 0; i < 4; ++i)
	{
		const int32 dC = PIECE_SHAPES[NextPieceType][0][i].dCol;
		const int32 dR = PIECE_SHAPES[NextPieceType][0][i].dRow;

		const FVector BlockLoc = PreviewCenter + FVector(0.0f, dC * CELL_SIZE, dR * CELL_SIZE);
		NextPieceBlocks[i]->SetVisibility(true);
		NextPieceBlocks[i]->SetRelativeLocation(BlockLoc);
		if (NextMat)
		{
			NextPieceBlocks[i]->SetMaterial(0, NextMat);
		}
	}
}

void ATetrisBoardActor::RebuildGridVisuals()
{
	// Clear all ISMs
	for (int32 i = 1; i <= 7; ++i)
	{
		PieceISMs[i]->ClearInstances();
	}

	// Add instances for all locked cells in local space
	for (int32 r = 0; r < GRID_ROWS; ++r)
	{
		for (int32 c = 0; c < GRID_COLS; ++c)
		{
			const int32 Type = Grid[r][c];
			if (Type >= 1 && Type <= 7)
			{
				FTransform InstanceTransform(FRotator::ZeroRotator, GridToLocalLocation(c, r), FVector(0.36f, 0.36f, 0.36f));
				PieceISMs[Type]->AddInstance(InstanceTransform);
			}
		}
	}
}

FVector ATetrisBoardActor::GridToLocalLocation(int32 Col, int32 Row) const
{
	const float LocalY = (static_cast<float>(Col) - 4.5f) * CELL_SIZE;
	const float LocalZ = (static_cast<float>(Row) + 0.5f) * CELL_SIZE;
	return FVector(0.0f, LocalY, LocalZ);
}

FLinearColor ATetrisBoardActor::GetPieceColor(int32 PieceType) const
{
	switch (PieceType)
	{
	case 1: return FLinearColor(0.0f, 0.85f, 0.95f);  // Cyan (I)
	case 2: return FLinearColor(0.95f, 0.85f, 0.05f); // Yellow (O)
	case 3: return FLinearColor(0.7f, 0.15f, 0.85f);  // Purple (T)
	case 4: return FLinearColor(0.15f, 0.85f, 0.2f);  // Green (S)
	case 5: return FLinearColor(0.95f, 0.15f, 0.15f); // Red (Z)
	case 6: return FLinearColor(0.15f, 0.35f, 0.95f); // Blue (J)
	case 7: return FLinearColor(1.0f, 0.5f, 0.05f);   // Orange (L)
	default: return FLinearColor::White;
	}
}
