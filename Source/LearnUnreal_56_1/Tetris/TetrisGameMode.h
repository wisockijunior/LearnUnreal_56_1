// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TetrisGameMode.generated.h"

class ATetrisBoardActor;

UCLASS()
class LEARNUNREAL_56_1_API ATetrisGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATetrisGameMode();

	virtual void BeginPlay() override;

	ATetrisBoardActor* GetBoardActor() const { return BoardActor; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tetris")
	TSubclassOf<ATetrisBoardActor> BoardClass;

private:
	UPROPERTY()
	ATetrisBoardActor* BoardActor;
};
