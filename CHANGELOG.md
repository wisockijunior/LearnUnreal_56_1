# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-10-04

### Added
- **Flappy Bird C++ Game** (`Source/LearnUnreal_56_1/Flappy/`):
  - `AFlappyBirdPawn`: Physics-based pawn simulation with upward impulse, downward gravity acceleration, pitch/roll tilt, sphere collision, and locked side-view camera.
  - `AFlappyPipePair`: Moving obstacle pair with dynamic height gaps accounting for center pivots, score overlap triggers, and off-screen lifecycle cleanup.
  - `AFlappyGameMode`: Game state machine (`Ready`, `Playing`, `GameOver`), procedural ground and sky backdrop spawner, score tracker, and randomized pipe spawner.
  - `AFlappyHUD`: Direct C++ canvas rendering with shadow-rendered text for real-time scores, high scores, and game over overlays.

- **Tetris C++ Game** (`Source/LearnUnreal_56_1/Tetris/`):
  - `ATetrisBoardActor`: $10 \times 20$ grid matrix, 7 standard tetromino shapes (`I`, `O`, `T`, `S`, `Z`, `J`, `L`) with 4 rotation states, wall-kick checks, line clearing, level progression, and drop speed acceleration.
  - Block rendering powered by `UInstancedStaticMeshComponent` for 60fps locked block rendering and dynamic colored materials.
  - `ATetrisPawn`: Frontal orthographic-perspective camera with input routing for movement, soft drop, hard drop, rotation, and restart.
  - `ATetrisGameMode`: Level initializer and board actor manager.
  - `ATetrisHUD`: On-screen HUD displaying score, level, lines cleared, next piece preview, and controls reference guide.

- **Project Configuration & Git**:
  - Comprehensive `.gitignore` targeting Unreal Engine 5.6 (excluding `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/`, Visual Studio / IDE temporary files, while tracking `Source/`, `Config/`, `Content/`, and `*.uproject`).
  - Updated `LearnUnreal_56_1.Build.cs` to include `SlateCore` and public include paths for `Flappy` and `Tetris`.

- **Documentation Suite** (`Docs/`):
  - `Docs/SCENE_SETUP_AND_PLAY_GUIDE.md`: Step-by-step scene creation, lighting, GameMode override assignment in World Settings, PIE testing, and Blueprint parameter customization guide for senior Unity developers.
  - `Docs/README.md`: Central documentation index and control cheat sheet.
  - `TUTORIAL_UNITY_TO_UNREAL.md`: Rosetta Stone mapping Unity concepts (`GameObject`, `MonoBehaviour`, `Update`, `[SerializeField]`, `Vector3`, `GameManager`) directly to Unreal Engine C++ equivalents (`AActor`, `UActorComponent`, `Tick`, `UPROPERTY`, `FVector`, `AGameModeBase`).

### Changed
- Moved TopDown template C++ classes (`LearnUnreal_56_1Character`, `LearnUnreal_56_1GameMode`, `LearnUnreal_56_1PlayerController`) into dedicated `Source/LearnUnreal_56_1/TopDown/` folder and updated `PublicIncludePaths` in `LearnUnreal_56_1.Build.cs`.

### Fixed
- Fixed compilation error `C2660: 'FGenericPlatformMath::SRand': function does not take 1 arguments` in `ATetrisBoardActor.cpp` by replacing it with `FMath::RandInit(FPlatformTime::Cycles())`.
- Fixed Tetris piece duplication bug when pressing Space (Hard Drop) by removing duplicate input polling in `ATetrisPawn::Tick`, adding debounce to `HardDrop`, resetting `DropTimer` on piece lock, and standardizing all active/locked block transforms to component-relative coordinates (`GridToLocalLocation`).
- Added Automated Testing Suite for Tetris simulation:
  - Added slot query methods (`GetOccupiedSlotCount`, `GetFreeSlotCount`) and spawning control (`SetSpawningEnabled`, `SpawnSpecificPiece`).
  - Added in-game automated test execution (`RunAutomatedTest`) bound to key `[T]` and console command.
  - Added Unreal Engine Automation Test (`FTetrisDropSimulationTest` in `TetrisAutomationTest.cpp`).
  - Added standalone verification tool (`Docs/test_tetris_simulation.cpp`) confirming 4 occupied / 196 free after 1st drop, and 8 occupied / 192 free after 2nd drop.
- Added Tetris HUD Drop Counter & Input Release-Gate:
  - Added `DROPS` statistic counter to `ATetrisHUD` directly below `LINES` and exposed `GetDropCount()` from `ATetrisBoardActor`.
  - Added strict key release-gate (`bCanHardDrop` reset on `IE_Released`) to `ATetrisPawn` preventing repeated drops while Space or Enter is held.
  - Cleared UnrealBuildTool `Log.txt` file lock to allow Live Coding compilation.

