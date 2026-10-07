// Copyright Renato / LearnUnreal_56_1. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "TetrisBoardActor.h"
#include "Engine/World.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTetrisDropSimulationTest, "LearnUnreal.Tetris.DropSimulation", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTetrisDropSimulationTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("World creation"), World))
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATetrisBoardActor* Board = World->SpawnActor<ATetrisBoardActor>(ATetrisBoardActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Board actor spawned"), Board))
	{
		World->DestroyWorld(false);
		return false;
	}

	// Run the full simulation test
	const bool bPassed = Board->RunAutomatedTest();
	TestTrue(TEXT("Automated simulation passed"), bPassed);

	// Verify exact slot counts after the 2 simulated drops
	TestEqual(TEXT("Occupied slots after two drops (exactly 2 pieces)"), Board->GetOccupiedSlotCount(), 8);
	TestEqual(TEXT("Free slots after two drops"), Board->GetFreeSlotCount(), 192);

	Board->Destroy();
	World->DestroyWorld(false);

	return bPassed;
}

#endif
