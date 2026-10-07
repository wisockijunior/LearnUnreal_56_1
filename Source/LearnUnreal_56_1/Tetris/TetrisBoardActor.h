// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TetrisBoardActor.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMeshComponent;

UCLASS()
class LEARNUNREAL_56_1_API ATetrisBoardActor : public AActor
{
	GENERATED_BODY()

public:
	static constexpr int32 GRID_COLS = 10;
	static constexpr int32 GRID_ROWS = 20;
	static constexpr float CELL_SIZE = 38.0f;

	ATetrisBoardActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Input actions
	void MoveLeft();
	void MoveRight();
	void RotatePiece();
	void SoftDrop();
	void HardDrop();
	void RestartGame();

	// Getters for HUD
	int32 GetScore() const { return Score; }
	int32 GetLinesCleared() const { return LinesCleared; }
	int32 GetLevel() const { return Level; }
	int32 GetNextPieceType() const { return NextPieceType; }
	bool IsGameOver() const { return bGameOver; }

	// Automated testing methods
	UFUNCTION(BlueprintCallable, Category = "Tetris|Testing")
	int32 GetOccupiedSlotCount() const;

	UFUNCTION(BlueprintCallable, Category = "Tetris|Testing")
	int32 GetFreeSlotCount() const;

	UFUNCTION(BlueprintCallable, Category = "Tetris|Testing")
	void SetSpawningEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Tetris|Testing")
	void SpawnSpecificPiece(int32 PieceType);

	UFUNCTION(BlueprintCallable, Exec, Category = "Tetris|Testing")
	bool RunAutomatedTest();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UInstancedStaticMeshComponent*> PieceISMs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UStaticMeshComponent*> ActivePieceBlocks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UStaticMeshComponent*> NextPieceBlocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tetris|Config")
	float BaseDropInterval;

private:
	void InitBoardVisuals();
	void CreateFrame();
	void SpawnNewPiece();
	bool IsValidPosition(int32 PieceType, int32 Rotation, int32 Col, int32 Row) const;
	void LockPiece();
	void ClearLines();
	void UpdateActivePieceVisuals();
	void UpdateNextPieceVisuals();
	void RebuildGridVisuals();
	FVector GridToLocalLocation(int32 Col, int32 Row) const;
	FLinearColor GetPieceColor(int32 PieceType) const;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> PieceMaterials;

	// Board state: 0 = empty, 1..7 = piece type color
	int32 Grid[GRID_ROWS][GRID_COLS];

	int32 CurrentPieceType;
	int32 CurrentRotation;
	int32 CurrentCol;
	int32 CurrentRow;

	int32 NextPieceType;

	float DropTimer;
	float CurrentDropInterval;

	int32 Score;
	int32 LinesCleared;
	int32 Level;
	bool bGameOver;
	bool bSpawningEnabled;

	UPROPERTY()
	UStaticMesh* CubeMesh;

	UPROPERTY()
	UMaterialInterface* BaseMaterial;
};
