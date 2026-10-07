// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TetrisPawn.generated.h"

class UCameraComponent;
class ATetrisBoardActor;

UCLASS()
class LEARNUNREAL_56_1_API ATetrisPawn : public APawn
{
	GENERATED_BODY()

public:
	ATetrisPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveLeft();
	void MoveRight();
	void RotatePiece();
	void SoftDrop();
	void HardDrop();
	void OnHardDropPressed();
	void OnHardDropReleased();
	void RestartGame();
	void TriggerAutomatedTest();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* OrthoCamera;

private:
	ATetrisBoardActor* GetBoardActor();

	UPROPERTY()
	ATetrisBoardActor* CachedBoard;

	// --- Hard Drop duplicate-press protection (see Docs/TETRIS_HARD_DROP_FIX.md) ---
	// Layer 2: time-based debounce. Ignores a second HardDrop within 0.2s of the last one.
	double LastHardDropTime;
	// Layer 3: key release-gate. Set false on IE_Pressed, true again only on IE_Released,
	// so one physical key press == exactly one drop (immune to OS key-repeat / held keys).
	bool bCanHardDrop;
};
