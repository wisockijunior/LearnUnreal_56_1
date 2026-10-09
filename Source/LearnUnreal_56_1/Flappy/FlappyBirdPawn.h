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

	/** Visual root holding all bird geometry for unified tilt, squash, and stretch */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* BirdVisualRoot;

	/** Main body (yellow sphere) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BirdMesh;

	/** Orange beak pointing forward (+Y) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BeakMesh;

	/** Eye white */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EyeMesh;

	/** Eye pupil */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PupilMesh;

	/** Animated wing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WingMesh;

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

	// Animation & Game Feel
	float BobTimer;
	float SquashStretchTimer;
	float CurrentTiltAngle;
	float WingFlapTime;
	float DeathSpinAngle;
};
