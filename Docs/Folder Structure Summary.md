Ran command: `Get-ChildItem -Path "Content" -Recurse -Filter "*.uasset" | Where-Object { $_.FullName -notmatch '__External' } | Select-Object FullName`
Viewed LearnUnreal_56_1Character.h:1-30
Viewed LearnUnreal_56_1PlayerController.h:1-25

The classes for **`Lvl_TopDown`** are actually sitting **right in front of you in the root folder** in that screenshot! 

Here is why they are named like that, how the template is organized, and what classes power each of the three tutorial levels.

---

### 1. Why `Lvl_TopDown` Classes Look Different

When you create a project in Unreal from a template, **Epic uses your Project Name (`LearnUnreal_56_1`) as the prefix for the base template classes**, and places them directly in the root of the module:

In your Content Browser (`All > C++ Classes > LearnUnreal_56_1`):
* 📁 `Variant_Strategy/`
* 📁 `Variant_TwinStick/`
* 👤 **`LearnUnreal_56_1Character`** $\leftarrow$ **This is the TopDown Character!**
* 🖥️ **`LearnUnreal_56_1GameMode`** $\leftarrow$ **This is the TopDown GameMode!**
* 🎮 **`LearnUnreal_56_1PlayerController`** $\leftarrow$ **This is the TopDown Controller!**

Because **TopDown is the primary game of this template**, Epic didn't put it in a subfolder; they put it in the root. Later, Epic added two bonus "Variants" (Strategy and TwinStick), and gave those subfolders.

---

### 2. Breakdown of the 3 Tutorial Levels

Here is how each level connects its **C++ Base Classes** to its **Content Blueprints** (Unreal's equivalent of Unity Prefabs):

---

#### 🟢 Level 1: `Lvl_TopDown` (Classic Point-and-Click / MOBA style)
* **Genre**: Diablo / League of Legends style point-and-click movement with pathfinding navigation.
* **C++ Location**: `Source/LearnUnreal_56_1/` (root)
* **Blueprints Location**: `Content/TopDown/Blueprints/`

| Role | C++ Base Class | Blueprint Subclass (`Content/`) | Responsibility |
| :--- | :--- | :--- | :--- |
| **GameMode** | `ALearnUnreal_56_1GameMode` | `BP_TopDownGameMode` | Sets rules, spawns player character, assigns controller. |
| **Pawn / Character** | `ALearnUnreal_56_1Character` | `BP_TopDownCharacter` | The mannequin character (`ACharacter`) with camera boom and top-down camera. |
| **Controller** | `ALearnUnreal_56_1PlayerController` | `BP_TopDownController` | Listens to mouse clicks, runs pathfinding (`UNavigationSystemV1`), spawns the green click cursor Niagara effect. |

---

#### 🔵 Level 2: `LVL_Strategy` (RTS / Command & Conquer style)
* **Genre**: Real-Time Strategy where you box-select multiple units and command them to move.
* **C++ Location**: `Source/LearnUnreal_56_1/Variant_Strategy/`
* **Blueprints Location**: `Content/Variant_Strategy/Blueprints/`

| Role | C++ Base Class | Blueprint Subclass (`Content/`) | Responsibility |
| :--- | :--- | :--- | :--- |
| **GameMode** | `AStrategyGameMode` | `BP_StrategyGameMode` | Handles RTS match state and rules. |
| **Camera Pawn** | `AStrategyPawn` | `BP_StrategyPawn` | A floating camera that pans (WASD / screen edge) and zooms (mouse wheel). Has no physical body. |
| **Controller** | `AStrategyPlayerController` | `BP_StrategyPlayerController` | Handles marquee drag-selection box, click orders, and unit selection list. |
| **Minion / Unit** | `AStrategyUnit` | `BP_StrategyUnit` | The soldier character that receives move orders and follows navmesh paths. |
| **HUD / UI** | `AStrategyHUD` / `UStrategyUI` | `BP_StrategyHUD` / `UI_Strategy` | Draws the green selection rectangle on the screen. |

---

#### 🔴 Level 3: `LVL_TwinStick` (Action Shooter with StateTree AI)
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

### 3. Folder Structure Summary: Disk (`Source/`) vs Content Browser

```text
Source/LearnUnreal_56_1/
├── LearnUnreal_56_1.Build.cs              <-- Module build dependencies
│
├── LearnUnreal_56_1GameMode.*             <-- [Lvl_TopDown] Base GameMode
├── LearnUnreal_56_1Character.*            <-- [Lvl_TopDown] Base Character
├── LearnUnreal_56_1PlayerController.*     <-- [Lvl_TopDown] Base Controller
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
├── Flappy/                                <-- [Our New Game] Flappy Bird
│   ├── FlappyGameMode.*
│   ├── FlappyBirdPawn.*
│   ├── FlappyPipePair.*
│   └── FlappyHUD.*
│
└── Tetris/                                <-- [Our New Game] Tetris
    ├── TetrisGameMode.*
    ├── TetrisBoardActor.*
    ├── TetrisPawn.*
    └── TetrisHUD.*
```

### Key Takeaway for Unity Devs:
Whenever you look at a Blueprint in `Content/` (e.g. `BP_TopDownCharacter`), you can look at the top-right corner of its Blueprint Editor window: it will say **Parent class: LearnUnreal_56_1Character**. Clicking that link opens the C++ header directly in Visual Studio!