// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlappyPipePair.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class LEARNUNREAL_56_1_API AFlappyPipePair : public AActor
{
	GENERATED_BODY()

public:
	AFlappyPipePair();

	virtual void Tick(float DeltaTime) override;

	/** Configure vertical gap position and gap size */
	void SetupPipes(float InGapCenterZ, float InGapSize, float InSpeed);

	/** 
	 * Trigger overlap callback: Fires when an actor enters the invisible scoring box between the pipes.
	 * (Unity equivalent: OnTriggerEnter(Collider other)).
	 * Must be marked UFUNCTION() to bind to the OnComponentBeginOverlap dynamic multicast delegate.
	 */
	UFUNCTION()
	void OnScoreTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when bird hits a pipe */
	UFUNCTION()
	void OnPipeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TopPipeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BottomPipeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* ScoreTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Pipe")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Pipe")
	float PipeLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Pipe")
	float PipeDiameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Pipe")
	float DestroyYThreshold;

private:
	bool bScoreGiven;
};
