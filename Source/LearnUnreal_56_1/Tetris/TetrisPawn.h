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
	void RestartGame();

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
};
