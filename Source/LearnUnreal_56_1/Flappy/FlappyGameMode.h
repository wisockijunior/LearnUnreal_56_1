// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FlappyGameMode.generated.h"

class AFlappyPipePair;
class AFlappyBirdPawn;

UENUM(BlueprintType)
enum class EFlappyGameState : uint8
{
	Ready,
	Playing,
	GameOver
};

UCLASS()
class LEARNUNREAL_56_1_API AFlappyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFlappyGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void StartGame();
	void OnBirdDied();
	void AddScore(int32 Amount = 1);
	void RestartGame();

	EFlappyGameState GetFlappyState() const { return CurrentState; }
	int32 GetCurrentScore() const { return CurrentScore; }
	int32 GetHighScore() const { return HighScore; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	TSubclassOf<AFlappyPipePair> PipePairClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	float PipeSpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	float PipeSpawnY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	float MinGapZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	float MaxGapZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	float GapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Config")
	float PipeSpeed;

	UFUNCTION()
	void SpawnPipePair();

private:
	void SpawnScenery();

	EFlappyGameState CurrentState;
	int32 CurrentScore;
	int32 HighScore;
	FTimerHandle TimerHandle_PipeSpawner;

	UPROPERTY()
	TArray<AActor*> SpawnedPipes;

	UPROPERTY()
	AActor* FloorActor;

	UPROPERTY()
	AActor* BackdropActor;
};
