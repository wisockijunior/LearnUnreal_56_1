// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlappyBirdPawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UCameraComponent;

UCLASS()
class LEARNUNREAL_56_1_API AFlappyBirdPawn : public APawn
{
	GENERATED_BODY()

public:
	AFlappyBirdPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Flap();
	void Die();
	void RequestRestart();

	bool IsDead() const { return bIsDead; }
	void ResetBird();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* SideViewCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Physics")
	float FlapStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Physics")
	float Gravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Bounds")
	float FloorZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flappy|Bounds")
	float CeilingZ;

	UPROPERTY()
	UMaterialInterface* BaseMaterial;

private:
	float VerticalVelocity;
	bool bIsDead;
	FVector StartLocation;
};
