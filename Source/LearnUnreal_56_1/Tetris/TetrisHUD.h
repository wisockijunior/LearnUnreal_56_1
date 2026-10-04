// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TetrisHUD.generated.h"

class ATetrisBoardActor;

UCLASS()
class LEARNUNREAL_56_1_API ATetrisHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATetrisHUD();

	virtual void DrawHUD() override;

private:
	void DrawCenteredString(const FString& Text, float CenterX, float CenterY, const FLinearColor& Color, float Scale = 1.0f);
	void DrawShadowedText(const FString& Text, float ScreenX, float ScreenY, const FLinearColor& Color, float Scale = 1.0f);

	ATetrisBoardActor* GetBoardActor();

	UPROPERTY()
	ATetrisBoardActor* CachedBoard;
};
