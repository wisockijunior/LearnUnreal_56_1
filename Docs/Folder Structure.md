# Project Folder Structure & Architecture Reference

This document provides a comprehensive map of the entire **LearnUnreal_56_1** repository, detailing the root workspace layout, Unreal Engine 5.6 C++ source code, Content Browser assets, and cross-platform learning implementations.

---

## 1. High-Level Workspace Directory Tree

```text
c:\Unreal Projects\LearnUnreal_56_1\
├── .gitignore                           # Git ignore rules for UE 5.6, VS, Python, Java, and J2ME
├── ARCHITECTURE_COMPARISON.md           # Side-by-side architecture comparison (UE5 C++, Unity, Python, Java)
├── CHANGELOG.md                         # Semantic versioning changelog of project milestones
├── LearnUnreal_56_1.sln                 # Visual Studio solution file
├── LearnUnreal_56_1.uproject            # Unreal Engine 5.6 project descriptor file
├── TUTORIAL_UNITY_TO_UNREAL.md          # Rosetta Stone guide for Unity developers learning Unreal C++
│
├── Config/                              # Unreal Engine project configuration (.ini)
│   ├── DefaultEditor.ini
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   └── DefaultInput.ini
│
├── Content/                             # Unreal Engine binary assets (.uasset, .umap)
│   ├── levels/                          # Tutorial & game level maps (.umap)
│   │   ├── NewMap1.umap                 # Playground test map
│   │   ├── NewMap2_Basic_FlappyBird.umap # Playable Flappy Bird level map
│   │   └── NewMap3_Tetris.umap          # Playable Tetris level map
│   ├── TopDown/                         # Blueprints, maps, and inputs for Point-and-Click
│   ├── Variant_Strategy/                # Blueprints and maps for RTS Strategy variant
│   └── Variant_TwinStick/               # Blueprints, maps, StateTree AI for Twin-Stick Shooter

│
├── Docs/                                # Central documentation suite
│   ├── README.md                        # Documentation index and quick reference
│   ├── Folder Structure.md              # (This document) Comprehensive repository map
│   ├── Folder Structure Summary.md      # Summary breakdown of C++ classes and levels
│   └── SCENE_SETUP_AND_PLAY_GUIDE.md    # Step-by-step scene creation & PIE testing guide
│
├── Games_Go/                            # Zero-dependency Go (Golang) games
│   ├── FlappyBird/                      # Standalone 60 FPS physics Flappy Bird (main.go)
│   ├── Tetris/                          # 10x20 grid Tetris with ghost piece (main.go)
│   └── README.md
│
├── Games_Java/                          # Standalone desktop Java 11 (Swing / AWT) games
│   ├── FlappyBird/                      # Double-buffered 60 FPS Flappy Bird GUI + run.bat
│   ├── Tetris/                          # 10x20 grid Tetris with ghost piece + run.bat
│   └── README.md
│
├── Games_JavaME_NokiaE63/               # J2ME MIDP 2.0 / CLDC 1.1 for Nokia E63 (Symbian S60)
│   ├── Common/                          # Clean MIDP 2.0 / CLDC 1.1 API stubs (MIDlet, Canvas, Graphics, RMS)
│   ├── FlappyBird/                      # 320x240 landscape Flappy Bird MIDlet + JAD/MANIFEST
│   ├── Tetris/                          # 320x240 landscape Tetris MIDlet + JAD/MANIFEST
│   ├── Emulator/                        # Interactive Nokia E63 hardware skin & desktop runner
│   ├── dist/                            # Ready-to-install FlappyBird.jar and Tetris.jar (~6 KB)
│   ├── build_all.bat                    # Compiles all MIDlets, builds simulator, packages JARs
│   ├── package_jars.bat                 # Generates .jar files and updates .jad byte counts
│   ├── run_nokia_e63_flappy.bat         # One-click desktop Nokia E63 Flappy simulator
│   ├── run_nokia_e63_tetris.bat         # One-click desktop Nokia E63 Tetris simulator
│   └── README.md
│
├── Games_Python/                        # Zero-dependency Python 3 (Tkinter) games
│   ├── FlappyBird/                      # Standalone 60 FPS physics Flappy Bird GUI (flappy_bird.py)
│   ├── Tetris/                          # 10x20 grid Tetris with ghost piece & preview (tetris.py)
│   └── README.md
│
├── Games_Rust/                          # Pure Rust (Systems, Zero-GC, Memory-Safe) games
│   ├── FlappyBird/                      # 60 FPS physics loop, AABB collision, ANSI TUI
│   ├── Tetris/                          # 10x20 grid, 7 tetrominoes, wall kicks, ghost piece
│   └── README.md
│
├── Games_Unity_CSharp/                  # Unity3D C# implementations
│   ├── FlappyBird/Scripts/              # Modular scripts with center-pivot offset calculations
│   ├── Tetris/Scripts/                  # 10x20 matrix, rotation states, immediate-mode OnGUI
│   └── README.md
│
├── Games_Verse/                         # Unreal Engine / UEFN (Verse Language) games
│   ├── FlappyBird/                      # Creative device: async physics loop, dynamic pipes
│   ├── Tetris/                          # Creative device: 10x20 grid, failable tests (<decides>)
│   └── README.md
│
└── Source/                              # Unreal Engine 5.6 C++ Source Code

    └── LearnUnreal_56_1/
        ├── LearnUnreal_56_1.Build.cs    # UBT module build rules and include paths
        ├── LearnUnreal_56_1.cpp         # Primary game module implementation
        ├── LearnUnreal_56_1.h           # Precompiled module header
        ├── TopDown/                     # [Level 1] Point-and-Click / MOBA style
        ├── Variant_Strategy/            # [Level 2] Real-Time Strategy (RTS)
        ├── Variant_TwinStick/           # [Level 3] Twin-Stick Shooter with StateTree AI
        ├── Flappy/                      # [Level 4] Flappy Bird arcade game
        └── Tetris/                      # [Level 5] Tetris arcade game
```

---

## 2. Unreal Engine C++ Source Directory (`Source/LearnUnreal_56_1/`)

The primary C++ module is organized into five distinct game modes and mini-games:

```text
Source/LearnUnreal_56_1/
├── LearnUnreal_56_1.Build.cs              <-- Module build dependencies (Core, Engine, SlateCore, etc.)
├── LearnUnreal_56_1.cpp                   <-- IMPLEMENT_PRIMARY_GAME_MODULE macro
├── LearnUnreal_56_1.h                     <-- Shared module header
│
├── TopDown/                               <-- [Lvl_TopDown] Point-and-Click Game
│   ├── LearnUnreal_56_1GameMode.h/.cpp    <-- Match rules, spawns player character & controller
│   ├── LearnUnreal_56_1Character.h/.cpp   <-- Top-down mannequin with camera boom
│   └── LearnUnreal_56_1PlayerController.h/.cpp <-- NavMesh pathfinding & Niagara click cursor
│
├── Variant_Strategy/                      <-- [LVL_Strategy] RTS Command & Conquer Style
│   ├── StrategyGameMode.h/.cpp            <-- RTS match rules and state
│   ├── StrategyPawn.h/.cpp                <-- Edge-panning, zooming RTS camera pawn
│   ├── StrategyPlayerController.h/.cpp    <-- Marquee box-selection & click move orders
│   ├── StrategyUnit.h/.cpp                <-- Minion unit character following nav paths
│   └── UI/
│       ├── StrategyHUD.h/.cpp             <-- Canvas marquee selection box renderer
│       └── StrategyUI.h/.cpp              <-- Slate / UMG UI bridge
│
├── Variant_TwinStick/                     <-- [LVL_TwinStick] Top-Down Action Shooter
│   ├── TwinStickGameMode.h/.cpp           <-- Player lives, score, wave progression
│   ├── TwinStickCharacter.h/.cpp          <-- Hero character with dash ability & health
│   ├── TwinStickPlayerController.h/.cpp   <-- Dual-stick / WASD+Mouse input routing
│   ├── Gameplay/
│   │   ├── TwinStickProjectile.h/.cpp     <-- High-velocity bouncing energy bullets
│   │   ├── TwinStickPickup.h/.cpp         <-- Health and power-up collectibles
│   │   └── TwinStickAoEAttack.h/.cpp      <-- Area-of-effect damage explosions
│   ├── AI/
│   │   ├── TwinStickNPC.h/.cpp            <-- Enemy character actor
│   │   ├── TwinStickAIController.h/.cpp   <-- Enemy AI controller
│   │   ├── TwinStickSpawner.h/.cpp        <-- Procedural wave spawner
│   │   └── TwinStickStateTreeUtility.h/.cpp <-- StateTree AI logic utility
│   └── UI/
│       └── TwinStickUI.h/.cpp             <-- HUD and health bar widgets
│
├── Flappy/                                <-- [Arcade 1] 2.5D Flappy Bird
│   ├── FlappyGameMode.h/.cpp              <-- State machine (Ready/Playing/GameOver) & spawner
│   ├── FlappyBirdPawn.h/.cpp              <-- Upward impulse, gravity, pitch tilt, camera
│   ├── FlappyPipePair.h/.cpp              <-- Procedural pipes with center-pivot offsets
│   └── FlappyHUD.h/.cpp                   <-- Direct C++ Canvas text & score rendering
│
└── Tetris/                                <-- [Arcade 2] Classic Tetris
    ├── TetrisGameMode.h/.cpp              <-- Board initializer and input mode router
    ├── TetrisBoardActor.h/.cpp            <-- 10x20 grid, 7 shapes, ISMs, wall-kicks, line clear
    ├── TetrisPawn.h/.cpp                  <-- Orthographic board camera & player input
    └── TetrisHUD.h/.cpp                   <-- Real-time HUD (Score, Level, Lines, Next Preview)
```

---

## 3. C++ Classes to Content Blueprint Mapping

In Unreal Engine, C++ classes serve as the high-performance logic foundation, while **Blueprint Subclasses** (located in `Content/`) assign meshes, animations, sounds, and materials:

| Game / Level | Role | C++ Base Class | Blueprint Asset (`Content/`) |
| :--- | :--- | :--- | :--- |
| **TopDown** | GameMode | `ALearnUnreal_56_1GameMode` | `Content/TopDown/Blueprints/BP_TopDownGameMode` |
| | Character | `ALearnUnreal_56_1Character` | `Content/TopDown/Blueprints/BP_TopDownCharacter` |
| | Controller | `ALearnUnreal_56_1PlayerController` | `Content/TopDown/Blueprints/BP_TopDownController` |
| **Strategy** | GameMode | `AStrategyGameMode` | `Content/Variant_Strategy/Blueprints/BP_StrategyGameMode` |
| | Camera Pawn | `AStrategyPawn` | `Content/Variant_Strategy/Blueprints/BP_StrategyPawn` |
| | Controller | `AStrategyPlayerController` | `Content/Variant_Strategy/Blueprints/BP_StrategyPlayerController` |
| | Unit | `AStrategyUnit` | `Content/Variant_Strategy/Blueprints/BP_StrategyUnit` |
| | HUD | `AStrategyHUD` | `Content/Variant_Strategy/Blueprints/BP_StrategyHUD` |
| **TwinStick** | GameMode | `ATwinStickGameMode` | `Content/Variant_TwinStick/Blueprints/BP_TwinStickGameMode` |
| | Character | `ATwinStickCharacter` | `Content/Variant_TwinStick/Blueprints/BP_TwinStickCharacter` |
| | Controller | `ATwinStickPlayerController` | `Content/Variant_TwinStick/Blueprints/BP_TwinStickPlayerController` |
| | Enemy NPC | `ATwinStickNPC` | `Content/Variant_TwinStick/Blueprints/BP_TwinStickNPC` |
| | AI Controller | `ATwinStickAIController` | `Content/Variant_TwinStick/Blueprints/BP_TwinStickAIController` |
| | AI Logic | `TwinStickStateTreeUtility` | `Content/Variant_TwinStick/Blueprints/ST_TwinStickNPC` |
| **Flappy** | Level Map | `NewMap2_Basic_FlappyBird` | `Content/levels/NewMap2_Basic_FlappyBird.umap` |
| | GameMode | `AFlappyGameMode` | World Settings GameMode Override |
| | Bird Pawn | `AFlappyBirdPawn` | Procedural sphere mesh & dynamic materials |
| | Obstacle | `AFlappyPipePair` | Procedural cylinder meshes & center-pivot math |
| | HUD | `AFlappyHUD` | Native Canvas rendering with shadow text |
| **Tetris** | Level Map | `NewMap3_Tetris` | `Content/levels/NewMap3_Tetris.umap` |
| | GameMode | `ATetrisGameMode` | World Settings GameMode Override |
| | Board Actor | `ATetrisBoardActor` | `UInstancedStaticMeshComponent` block renderer |
| | Pawn | `ATetrisPawn` | Orthogonal player camera & input routing |
| | HUD | `ATetrisHUD` | Native Canvas rendering (Score, Next piece preview) |

---

## 4. Multi-Language Implementations Overview

To facilitate cross-platform study and engine comparison, both **Flappy Bird** and **Tetris** are implemented in three additional technology stacks:

### 1. Unity3D (C#) — [`Games_Unity_CSharp/`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Unity_CSharp/README.md)
* **Design**: Modular `MonoBehaviour` scripts designed for drag-and-drop into any empty Unity scene.
* **Pivot Alignment**: Enforces the **Unity Y-Coordinate & Center-Pivot Check** on primitives (`Cube`, `Cylinder`), adjusting local $Y$ by half-height offsets to prevent pipe/block clipping.
* **Zero-Setup HUD**: Uses `OnGUI()` for instant display without requiring UI Canvas prefabs.

### 2. Python 3 (Tkinter) — [`Games_Python/`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Python/README.md)
* **Design**: 100% pure standard library Python (`import tkinter`), running at locked 60 FPS via `root.after(16, loop)`.
* **Zero Dependencies**: Runs out of the box with `python flappy_bird.py` and `python tetris.py`.
* **Features**: Full ghost piece projection, next-piece preview box, soft/hard drops, and dynamic list line clears.

### 3. Java 11 (Swing / AWT) — [`Games_Java/`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Java/README.md)
* **Design**: Double-buffered `JPanel` rendering via `Graphics2D` with anti-aliasing and affine rotation transforms.
* **Loop**: Driven by `javax.swing.Timer` on the Event Dispatch Thread (EDT).
* **High Performance**: Employs `System.arraycopy` for zero-allocation row clears in Tetris.

### 4. Nokia E63 J2ME (MIDP 2.0 / CLDC 1.1) — [`Games_JavaME_NokiaE63/`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_JavaME_NokiaE63/README.md)
* **Target Hardware**: Nokia E63 smartphone (Symbian OS 9.2, S60 3rd Edition, 369 MHz ARM9 CPU).
* **Display Format**: **320 x 240 pixels** in **Horizontal Landscape**.
* **J2ME APIs**: LCDUI `Canvas`, `Graphics` with alignment anchors, `RecordStore` (RMS) persistent storage for high scores, and `Manager.playTone()` for retro 8-bit sound tones.
* **Desktop Simulator**: Includes an interactive **Nokia E63 Hardware Simulator** rendering the phone body, 320x240 LCD, 5-way Navi-Key D-Pad, and QWERTY keypad.
* **Pre-Packaged Binaries**: Ready-to-install `FlappyBird.jar` and `Tetris.jar` (~6 KB each) located in `dist/`.

---

## 5. Documentation Directory Index (`Docs/`)

| Document | Link | Description |
| :--- | :--- | :--- |
| **Project Documentation Index** | [`Docs/README.md`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/README.md) | Central navigation hub, control reference, and high-level project summary. |
| **Folder Structure & Architecture** | [`Docs/Folder Structure.md`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/Folder%20Structure.md) | (This file) Complete workspace tree, C++ module breakdown, Content mapping. |
| **Folder Structure Summary** | [`Docs/Folder Structure Summary.md`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/Folder%20Structure%20Summary.md) | Concise summary of Unreal C++ classes, Level mapping, and Content Browser layout. |
| **Scene Setup & Play Guide** | [`Docs/SCENE_SETUP_AND_PLAY_GUIDE.md`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/SCENE_SETUP_AND_PLAY_GUIDE.md) | Step-by-step instructions on creating `.umap` maps, lighting, World Settings, and PIE. |
| **Unity to Unreal Rosetta Stone** | [`TUTORIAL_UNITY_TO_UNREAL.md`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/TUTORIAL_UNITY_TO_UNREAL.md) | Conceptual translation guide comparing `GameObject` vs `AActor`, `Update` vs `Tick`, etc. |
| **Cross-Language Comparison** | [`ARCHITECTURE_COMPARISON.md`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/ARCHITECTURE_COMPARISON.md) | Deep comparative analysis across Unreal C++, Unity C#, Python, and Java. |
