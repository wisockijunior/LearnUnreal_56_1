# Project Folder Structure & Level Classes Breakdown

This document explains the organization of the C++ codebase, how classes are mapped to each tutorial level and game mode, and how the Unreal Content Browser represents them.

---

## 1. Why `Lvl_TopDown` Classes Look Different

When you create an Unreal project using Epic's **Top Down Template**, the wizard automatically uses your **Project Name (`LearnUnreal_56_1`)** as the prefix for the core template classes:
* `ALearnUnreal_56_1Character`
* `ALearnUnreal_56_1GameMode`
* `ALearnUnreal_56_1PlayerController`

Originally, Epic placed these three files directly in the root of the C++ module. They have now been organized into their own dedicated folder:
📁 **`Source/LearnUnreal_56_1/TopDown/`**

---

## 2. Complete Level & Game Mode Breakdown

Here is how each level and game connects its **C++ Base Classes** to its **Content Blueprints** (Unreal's equivalent of Unity Prefabs):

---

### 🟢 Level 1: `Lvl_TopDown` (Point-and-Click / MOBA Style)
* **Genre**: Diablo / League of Legends style point-and-click movement with pathfinding navigation.
* **C++ Location**: `Source/LearnUnreal_56_1/TopDown/`
* **Blueprints Location**: `Content/TopDown/Blueprints/`

| Role | C++ Base Class | Blueprint Subclass (`Content/`) | Responsibility |
| :--- | :--- | :--- | :--- |
| **GameMode** | `ALearnUnreal_56_1GameMode` | `BP_TopDownGameMode` | Defines match rules, spawns player character, assigns player controller. |
| **Character** | `ALearnUnreal_56_1Character` | `BP_TopDownCharacter` | The mannequin character (`ACharacter`) equipped with camera boom and top-down camera component. |
| **Controller** | `ALearnUnreal_56_1PlayerController` | `BP_TopDownController` | Listens to mouse clicks, calculates pathfinding (`UNavigationSystemV1`), and spawns the green click cursor Niagara effect. |

---

### 🔵 Level 2: `LVL_Strategy` (RTS / Command & Conquer Style)
* **Genre**: Real-Time Strategy where you box-select multiple units and command them to move.
* **C++ Location**: `Source/LearnUnreal_56_1/Variant_Strategy/`
* **Blueprints Location**: `Content/Variant_Strategy/Blueprints/`

| Role | C++ Base Class | Blueprint Subclass (`Content/`) | Responsibility |
| :--- | :--- | :--- | :--- |
| **GameMode** | `AStrategyGameMode` | `BP_StrategyGameMode` | Handles RTS match state and rules. |
| **Camera Pawn** | `AStrategyPawn` | `BP_StrategyPawn` | A floating camera that pans (WASD / screen edge) and zooms (mouse wheel). Has no physical mesh. |
| **Controller** | `AStrategyPlayerController` | `BP_StrategyPlayerController` | Handles marquee drag-selection box, click orders, and unit selection list. |
| **Minion / Unit** | `AStrategyUnit` | `BP_StrategyUnit` | The soldier character that receives move orders and follows navmesh paths. |
| **HUD / UI** | `AStrategyHUD` / `UStrategyUI` | `BP_StrategyHUD` / `UI_Strategy` | Draws the green selection rectangle on the screen. |

---

### 🔴 Level 3: `LVL_TwinStick` (Action Shooter with StateTree AI)
* **Genre**: WASD movement + Mouse/Stick directional aim and shooting against enemy waves.
* **C++ Location**: `Source/LearnUnreal_56_1/Variant_TwinStick/`
* **Blueprints Location**: `Content/Variant_TwinStick/Blueprints/`

| Role | C++ Base Class | Blueprint Subclass (`Content/`) | Responsibility |
| :--- | :--- | :--- | :--- |
| **GameMode** | `ATwinStickGameMode` | `BP_TwinStickGameMode` | Tracks player lives, score, and spawns enemy waves. |
| **Character** | `ATwinStickCharacter` | `BP_TwinStickCharacter` | Twin-stick shooter hero with dash ability (`IA_Action_Dash`), projectile weapon firing, and health. |
| **Controller** | `ATwinStickPlayerController` | `BP_TwinStickPlayerController` | Dual-axis input routing (left stick = move, right stick/mouse = look direction). |
| **Projectile** | `ATwinStickProjectile` | `BP_TwinStickProjectile` | Energy bullets with collision, bounce, and damage. |
| **Pickups / AoE** | `ATwinStickPickup` / `ATwinStickAoEAttack` | `BP_TwinStickPickup` / `BP_TwinStickAoEAttack` | Collectible power-ups and area explosive damage. |
| **AI System** | `ATwinStickNPC` / `ATwinStickAIController` / `ATwinStickSpawner` | `BP_TwinStickNPC` / `ST_TwinStickNPC` | Enemy enemies running Unreal's new **StateTree** AI system (the modern UE5 replacement for Behavior Trees). |

---

### 🟡 Level 4: Flappy Bird (2.5D Side-Scroller)
* **Genre**: Physics-based tap-to-fly arcade game with moving pipes and scoring gates.
* **C++ Location**: `Source/LearnUnreal_56_1/Flappy/`

| Role | C++ Base Class | Responsibility |
| :--- | :--- | :--- |
| **GameMode** | `AFlappyGameMode` | Controls `Ready` $\rightarrow$ `Playing` $\rightarrow$ `GameOver` states, spawns pipe pairs via timer, tracks high score. |
| **Bird Pawn** | `AFlappyBirdPawn` | Physics velocity simulation (impulse flap, gravity fall, pitch/roll tilt), sphere collision, locked camera. |
| **Obstacle** | `AFlappyPipePair` | Top & bottom cylindrical pipes with center-pivot gap calculation and middle score trigger. |
| **HUD** | `AFlappyHUD` | Native C++ canvas drawing title, score, high score, and game over screen. |

---

### 🟣 Level 5: Tetris (Classic 10x20 Grid)
* **Genre**: Retro falling-block puzzle game with SRS-style rotation and line clearing.
* **C++ Location**: `Source/LearnUnreal_56_1/Tetris/`

| Role | C++ Base Class | Responsibility |
| :--- | :--- | :--- |
| **GameMode** | `ATetrisGameMode` | Spawns board actor, configures Game & UI input mode. |
| **Board Actor** | `ATetrisBoardActor` | $10 \times 20$ grid matrix, 7 tetrominoes, wall kicks, line clearing, instanced static mesh block rendering. |
| **Pawn** | `ATetrisPawn` | Fixed orthogonal camera facing the board; routes player input (A/D, W, S, Space, R). |
| **HUD** | `ATetrisHUD` | Canvas HUD showing score, level, lines cleared, next piece preview, and controls guide. |

---

## 3. Directory Structure: Disk (`Source/`) vs Content Browser

```text
Source/LearnUnreal_56_1/
├── LearnUnreal_56_1.Build.cs              <-- Module build dependencies & include paths
├── LearnUnreal_56_1.cpp                   <-- Module implementation (PRIMARY_GAME_MODULE)
├── LearnUnreal_56_1.h                     <-- Module precompiled header
│
├── TopDown/                               <-- [Lvl_TopDown] Point-and-Click Game
│   ├── LearnUnreal_56_1GameMode.*
│   ├── LearnUnreal_56_1Character.*
│   └── LearnUnreal_56_1PlayerController.*
│
├── Variant_Strategy/                      <-- [LVL_Strategy] RTS Classes
│   ├── StrategyGameMode.*
│   ├── StrategyPawn.*
│   ├── StrategyPlayerController.*
│   ├── StrategyUnit.*
│   └── UI/ (StrategyHUD.*, StrategyUI.*)
│
├── Variant_TwinStick/                     <-- [LVL_TwinStick] Twin-Stick Shooter
│   ├── TwinStickGameMode.*
│   ├── TwinStickCharacter.*
│   ├── TwinStickPlayerController.*
│   ├── Gameplay/ (Projectile, AoE, Pickups)
│   ├── AI/ (NPC, AIController, Spawner, StateTree)
│   └── UI/ (TwinStickUI.*)
│
├── Flappy/                                <-- [Arcade] Flappy Bird
│   ├── FlappyGameMode.*
│   ├── FlappyBirdPawn.*
│   ├── FlappyPipePair.*
│   └── FlappyHUD.*
│
└── Tetris/                                <-- [Arcade] Tetris
    ├── TetrisGameMode.*
    ├── TetrisBoardActor.*
    ├── TetrisPawn.*
    └── TetrisHUD.*
```

In the **Content Browser** (`All > C++ Classes > LearnUnreal_56_1`), you will see these exact folders cleanly mirrored:
* 📁 `TopDown/`
* 📁 `Variant_Strategy/`
* 📁 `Variant_TwinStick/`
* 📁 `Flappy/`
* 📁 `Tetris/`

---

## 4. Key Takeaways for Unity Developers

1. **C++ Base vs Blueprint Subclass**:
   In Unity, you write a C# script and attach it to a GameObject or Prefab. In Unreal, you write a C++ class (e.g. `ALearnUnreal_56_1Character`) and subclass it as a Blueprint asset (`BP_TopDownCharacter`) to assign meshes, animations, and materials visually in the Editor.
2. **Finding the C++ Class from a Blueprint**:
   Open any Blueprint in the Editor (e.g., `BP_TopDownCharacter`), look at the top-right corner of the window. It shows **Parent class: LearnUnreal_56_1Character**. Clicking that link opens the C++ header directly in your IDE.
3. **Module Include Paths**:
   Whenever a new C++ subfolder is added, add it to `PublicIncludePaths` in `LearnUnreal_56_1.Build.cs` so `#include` statements can find headers directly without relative `../../` paths.

---

## 5. Cross-Platform Implementations & Workspace Reference

For the complete tree covering the entire project root, assets, and cross-platform learning games, see [**Folder Structure.md**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/Folder%20Structure.md):

* 📁 [**`Games_Verse/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Verse/README.md) — Unreal Engine / UEFN Verse implementations using async loops, failable `<decides>`, and transactions `<transacts>`.
* 📁 [**`Games_Rust/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Rust/README.md) — Pure Rust implementations featuring ownership/borrowing, zero GC, and crossterm TUI rendering.
* 📁 [**`Games_Go/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Go/README.md) — Zero-dependency Go (Golang) implementations utilizing goroutines, tickers, and Windows console integration.
* 📁 [**`Games_Unity_CSharp/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Unity_CSharp/README.md) — Unity3D C# implementations with explicit center-pivot offset calculations.
* 📁 [**`Games_Python/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Python/README.md) — Standalone zero-dependency Python 3 Tkinter games (60 FPS).
* 📁 [**`Games_Java/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Java/README.md) — Desktop Java 11 Swing implementations with double buffering.
* 📁 [**`Games_JavaME_NokiaE63/`**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_JavaME_NokiaE63/README.md) — Authentic J2ME MIDP 2.0 / CLDC 1.1 games for Nokia E63 (320x240 landscape display, RMS flash persistence, hardware tone audio, and desktop simulator).