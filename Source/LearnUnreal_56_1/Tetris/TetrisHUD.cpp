// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "TetrisHUD.h"
#include "TetrisBoardActor.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ATetrisHUD::ATetrisHUD()
	: CachedBoard(nullptr)
{
}

ATetrisBoardActor* ATetrisHUD::GetBoardActor()
{
	if (!CachedBoard)
	{
		CachedBoard = Cast<ATetrisBoardActor>(UGameplayStatics::GetActorOfClass(this, ATetrisBoardActor::StaticClass()));
	}
	return CachedBoard;
}

void ATetrisHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	ATetrisBoardActor* Board = GetBoardActor();
	if (!Board)
	{
		return;
	}

	const float ScreenCenterX = Canvas->ClipX * 0.5f;
	const float ScreenCenterY = Canvas->ClipY * 0.5f;

	// Title
	DrawShadowedText(TEXT("TETRIS"), ScreenCenterX - 380.0f, 60.0f, FLinearColor(0.2f, 0.85f, 1.0f), 2.4f);

	// Score Stats Panel on Left
	const float StatLeft = ScreenCenterX - 380.0f;
	DrawShadowedText(TEXT("SCORE"), StatLeft, 140.0f, FLinearColor(0.7f, 0.7f, 0.7f), 1.2f);
	DrawShadowedText(FString::Printf(TEXT("%d"), Board->GetScore()), StatLeft, 170.0f, FLinearColor::White, 1.8f);

	DrawShadowedText(TEXT("LEVEL"), StatLeft, 230.0f, FLinearColor(0.7f, 0.7f, 0.7f), 1.2f);
	DrawShadowedText(FString::Printf(TEXT("%d"), Board->GetLevel()), StatLeft, 260.0f, FLinearColor(1.0f, 0.85f, 0.1f), 1.8f);

	DrawShadowedText(TEXT("LINES"), StatLeft, 320.0f, FLinearColor(0.7f, 0.7f, 0.7f), 1.2f);
	DrawShadowedText(FString::Printf(TEXT("%d"), Board->GetLinesCleared()), StatLeft, 350.0f, FLinearColor(0.3f, 1.0f, 0.4f), 1.8f);

	// Next Piece Label on Right
	DrawShadowedText(TEXT("NEXT"), ScreenCenterX + 210.0f, 140.0f, FLinearColor(1.0f, 0.85f, 0.1f), 1.4f);

	// Bottom Controls Help
	const FString ControlsText = TEXT("[A / D] Move    [W / Up] Rotate    [S / Down] Soft Drop    [Space] Hard Drop    [R] Restart");
	DrawCenteredString(ControlsText, ScreenCenterX, Canvas->ClipY - 45.0f, FLinearColor(0.85f, 0.85f, 0.85f), 1.05f);

	// Game Over Overlay
	if (Board->IsGameOver())
	{
		DrawCenteredString(TEXT("GAME OVER"), ScreenCenterX, ScreenCenterY - 80.0f, FLinearColor(1.0f, 0.2f, 0.2f), 3.0f);
		DrawCenteredString(FString::Printf(TEXT("Final Score: %d"), Board->GetScore()), ScreenCenterX, ScreenCenterY, FLinearColor::White, 1.8f);
		DrawCenteredString(TEXT("Press R to Play Again"), ScreenCenterX, ScreenCenterY + 60.0f, FLinearColor(0.3f, 1.0f, 0.3f), 1.4f);
	}
}

void ATetrisHUD::DrawCenteredString(const FString& Text, float CenterX, float CenterY, const FLinearColor& Color, float Scale)
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font) return;

	float TextWidth = 0.0f, TextHeight = 0.0f;
	GetTextSize(Text, TextWidth, TextHeight, Font, Scale);

	const float PosX = CenterX - (TextWidth * 0.5f);
	const float PosY = CenterY - (TextHeight * 0.5f);

	DrawShadowedText(Text, PosX, PosY, Color, Scale);
}

void ATetrisHUD::DrawShadowedText(const FString& Text, float ScreenX, float ScreenY, const FLinearColor& Color, float Scale)
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font) return;

	// Drop shadow
	DrawText(Text, FLinearColor(0.0f, 0.0f, 0.0f, 0.8f), ScreenX + 2.0f * Scale, ScreenY + 2.0f * Scale, Font, Scale);

	// Foreground text
	DrawText(Text, Color, ScreenX, ScreenY, Font, Scale);
}
