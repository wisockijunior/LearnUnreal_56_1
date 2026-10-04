// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FlappyHUD.generated.h"

UCLASS()
class LEARNUNREAL_56_1_API AFlappyHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFlappyHUD();

	virtual void DrawHUD() override;

private:
	void DrawCenteredString(const FString& Text, float CenterX, float CenterY, const FLinearColor& Color, float Scale = 1.0f);
};
