// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "FlappyHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "FlappyGameMode.h"

AFlappyHUD::AFlappyHUD()
{
}

void AFlappyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	AFlappyGameMode* GM = Cast<AFlappyGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM)
	{
		return;
	}

	const float ScreenCenterX = Canvas->ClipX * 0.5f;
	const float ScreenCenterY = Canvas->ClipY * 0.5f;

	switch (GM->GetFlappyState())
	{
	case EFlappyGameState::Ready:
		DrawCenteredString(TEXT("FLAPPY BIRD"), ScreenCenterX, ScreenCenterY - 140.0f, FLinearColor(1.0f, 0.85f, 0.1f), 2.5f);
		DrawCenteredString(TEXT("Press SPACE or CLICK to Flap & Start!"), ScreenCenterX, ScreenCenterY - 40.0f, FLinearColor::White, 1.4f);
		DrawCenteredString(FString::Printf(TEXT("High Score: %d"), GM->GetHighScore()), ScreenCenterX, ScreenCenterY + 40.0f, FLinearColor(0.8f, 0.8f, 0.8f), 1.3f);
		break;

	case EFlappyGameState::Playing:
		DrawCenteredString(FString::Printf(TEXT("%d"), GM->GetCurrentScore()), ScreenCenterX, 80.0f, FLinearColor::White, 3.0f);
		break;

	case EFlappyGameState::GameOver:
		DrawCenteredString(TEXT("GAME OVER"), ScreenCenterX, ScreenCenterY - 160.0f, FLinearColor(1.0f, 0.2f, 0.2f), 3.0f);
		DrawCenteredString(FString::Printf(TEXT("Score: %d"), GM->GetCurrentScore()), ScreenCenterX, ScreenCenterY - 60.0f, FLinearColor::White, 1.8f);
		DrawCenteredString(FString::Printf(TEXT("Best: %d"), GM->GetHighScore()), ScreenCenterX, ScreenCenterY, FLinearColor(1.0f, 0.85f, 0.1f), 1.8f);
		DrawCenteredString(TEXT("Press SPACE or R to Restart"), ScreenCenterX, ScreenCenterY + 90.0f, FLinearColor(0.4f, 1.0f, 0.4f), 1.4f);
		break;
	}
}

void AFlappyHUD::DrawCenteredString(const FString& Text, float CenterX, float CenterY, const FLinearColor& Color, float Scale)
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font)
	{
		return;
	}

	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	GetTextSize(Text, TextWidth, TextHeight, Font, Scale);

	const float PosX = CenterX - (TextWidth * 0.5f);
	const float PosY = CenterY - (TextHeight * 0.5f);

	// Shadow
	DrawText(Text, FLinearColor(0.0f, 0.0f, 0.0f, 0.7f), PosX + 2.0f * Scale, PosY + 2.0f * Scale, Font, Scale);

	// Main text
	DrawText(Text, Color, PosX, PosY, Font, Scale);
}
