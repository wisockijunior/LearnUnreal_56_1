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
