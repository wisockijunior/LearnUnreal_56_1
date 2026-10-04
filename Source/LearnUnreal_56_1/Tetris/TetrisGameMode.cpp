// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "TetrisGameMode.h"
#include "TetrisPawn.h"
#include "TetrisBoardActor.h"
#include "TetrisHUD.h"
#include "Kismet/GameplayStatics.h"

ATetrisGameMode::ATetrisGameMode()
	: BoardActor(nullptr)
{
	DefaultPawnClass = ATetrisPawn::StaticClass();
	HUDClass = ATetrisHUD::StaticClass();
	BoardClass = ATetrisBoardActor::StaticClass();
}

void ATetrisGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Check if already placed in the level
	BoardActor = Cast<ATetrisBoardActor>(UGameplayStatics::GetActorOfClass(this, ATetrisBoardActor::StaticClass()));
	if (!BoardActor && BoardClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		BoardActor = GetWorld()->SpawnActor<ATetrisBoardActor>(BoardClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
	}
}
