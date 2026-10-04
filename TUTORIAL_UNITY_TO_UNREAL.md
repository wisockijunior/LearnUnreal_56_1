# Unity Developer's Guide to Unreal Engine 5.6 C++
## Flappy Bird & Tetris C++ Architecture & Gameplay Tutorial

Welcome Renato! As a senior Unity developer diving into Unreal Engine 5, this project provides two complete, clean C++ game implementations: **Flappy Bird** and **Tetris**.

Both games are built purely in C++ using Unreal Engine's native primitives and dynamic materials, meaning **zero external asset dependencies, zero missing meshes, and 100% plug-and-play execution**.

---

## 1. The Rosetta Stone: Unity to Unreal Engine 5

| Unity Concept | Unreal Engine C++ Equivalent | Key Architectural Differences |
| :--- | :--- | :--- |
| **`GameObject`** | **`AActor`** | In Unreal, anything that can be placed in a level inherits from `AActor`. Prefixed with `A`. |
| **`MonoBehaviour` / Component** | **`UActorComponent` / `USceneComponent`** | Components that have a transform (position, rotation, scale) inherit from `USceneComponent`. Logic-only components inherit from `UActorComponent`. Prefixed with `U`. |
| **`Transform` & Hierarchy** | **`USceneComponent* RootComponent`** | In Unreal, an Actor does not have a transform on itself; its `RootComponent` defines its world transform. Other components attach to `RootComponent`. |
| **Coordinate System & Units** | **Left-Handed Z-Up (Centimeters)** | Unity is Left-Handed $Y$-Up ($1\text{ unit} = 1\text{ meter}$). Unreal is Left-Handed $Z$-Up ($1\text{ unit} = 1\text{ centimeter}$). $+X$ is Forward, $+Y$ is Right, $+Z$ is Up. |
| **Pivot Convention** | **Center Pivot for Engine Primitives** | Standard Unreal primitive meshes (`Cube`, `Sphere`, `Cylinder`) have their pivot at the geometric center `(0, 0, 0)`. When placing an $800\text{ cm}$ tall cylinder, its center is at half-height ($400\text{ cm}$). |
| **`Update()`** | **`Tick(float DeltaTime)`** | Enable with `PrimaryActorTick.bCanEverTick = true;`. DeltaTime is passed in seconds. |
| **`Awake()` / `Start()`** | **Constructor & `BeginPlay()`** | Component creation (`CreateDefaultSubobject`) occurs in the C++ constructor. Gameplay initialization occurs in `BeginPlay()`. |
| **`[SerializeField]`** | **`UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "...")`** | Exposes variables to the Unreal reflection system, Editor details panel, Blueprints, and garbage collection. |
| **`Instantiate(prefab)`** | **`GetWorld()->SpawnActor<T>(...)`** | Spawns an actor into the world. |
| **`Destroy(gameObject)`** | **`Actor->Destroy()`** | Marks actor for destruction and removes from level. |
| **`Vector3` / `Quaternion`** | **`FVector` / `FRotator` / `FQuat`** | `FRotator(Pitch, Yaw, Roll)` represents Euler angles (Pitch = Y rotation, Yaw = Z rotation, Roll = X rotation). |
| **`Coroutine`** | **`FTimerManager`** | `GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyClass::MyFunc, Rate, bLoop);` |
| **`GameManager`** | **`AGameModeBase` & `UGameInstance`** | `AGameModeBase` defines match rules, default pawns, and game flow for the level. `UGameInstance` persists across level loads. |
| **`PlayerInput` / Controls** | **`APlayerController` & `APawn`** | Unreal separates the "mind" (`APlayerController`) from the "body" (`APawn`). Input can be routed directly via `SetupPlayerInputComponent` or polled in `Tick`. |
| **UI Canvas** | **`AHUD` / UMG Slate** | `AHUD::DrawHUD()` provides direct 2D canvas drawing in C++ without requiring Widget Blueprints. |
| **Garbage Collection** | **Unreal Garbage Collector** | Unreal automatically garbage collects `UObject` pointers marked with `UPROPERTY()`. Non-UObjects use `TSharedPtr` / `TWeakPtr`. |

---

## 2. Flappy Bird Architecture (`Source/LearnUnreal_56_1/Flappy/`)

Flappy Bird operates as a 2.5D side-scrolling physics game positioned in the $Y$-$Z$ plane:
- $X$: Camera depth (Camera sits at $X = -850$, bird and pipes are at $X = 0$).
- $Y$: Horizontal direction (Pipes spawn at $+Y = 900$ and scroll to $-Y = -1200$).
- $Z$: Vertical height (Bird jumps in $+Z$, gravity pulls in $-Z$).

### Key Classes

1. **[`AFlappyBirdPawn`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Flappy/FlappyBirdPawn.h)**:
   - **Root**: `USphereComponent` collision sphere (radius $24\text{ cm}$).
   - **Visual**: `UStaticMeshComponent` using `/Engine/BasicShapes/Sphere.Sphere` with a bright yellow dynamic material.
   - **Camera**: `UCameraComponent` locked at $X = -850, Y = 0, Z = 0$ facing $+X$ using `SetUsingAbsoluteLocation(true)` to prevent screen jitter.
   - **Physics Simulation**:
     $$\text{VerticalVelocity} \mathrel{+}= \text{Gravity} \times \Delta t \quad (\text{Gravity} = -1500\text{ cm/s}^2)$$
     $$\text{NewZ} = \text{CurrentZ} + \text{VerticalVelocity} \times \Delta t$$
   - **Visual Rotation**: Roll angle is computed from vertical velocity, tilting the bird's beak up when jumping and down when descending.
   - **Controls**: Flap triggered on `SpaceBar`, `LeftMouseButton`, `Up`, or `W`.

2. **[`AFlappyPipePair`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Flappy/FlappyPipePair.h)**:
   - **Pipes**: Two cylindrical meshes (Top and Bottom) colored vibrant green.
   - **Center Pivot Mathematics**:
     $$\text{TopPipeZ} = \text{GapCenterZ} + \frac{\text{GapSize}}{2} + \frac{\text{PipeLength}}{2}$$
     $$\text{BottomPipeZ} = \text{GapCenterZ} - \frac{\text{GapSize}}{2} - \frac{\text{PipeLength}}{2}$$
   - **Score Trigger**: `UBoxComponent` positioned exactly inside the gap. Overlapping with the bird awards $+1$ point.
   - **Movement & Lifecycle**: Translates along $-Y$ at $340\text{ cm/s}$ and self-destructs when passing off-screen.

3. **[`AFlappyGameMode`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Flappy/FlappyGameMode.h)**:
   - Controls states: `Ready` $\rightarrow$ `Playing` $\rightarrow$ `GameOver`.
   - Spawns ground visual and sky backdrop dynamically at runtime.
   - Runs a repeating timer `TimerHandle_PipeSpawner` to spawn randomized pipe pairs.
   - Handles game reset and tracks high score.

4. **[`AFlappyHUD`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Flappy/FlappyHUD.h)**:
   - Draws HUD directly using `Canvas->DrawText` with drop shadows.
   - Displays Title, Start prompt, dynamic score, and Game Over banner.

---

## 3. Tetris Architecture (`Source/LearnUnreal_56_1/Tetris/`)

Classic $10 \times 20$ Tetris implemented with high-performance instanced static meshes and standard tetromino logic.

### Coordinate & Grid Mapping
- **Grid Size**: 10 columns $\times$ 20 rows.
- **Cell Dimension**: $38\text{ cm}$ per block.
- **World Position Conversion**:
  $$Y_{\text{world}} = (\text{Col} - 4.5) \times 38\text{ cm}$$
  $$Z_{\text{world}} = (\text{Row} + 0.5) \times 38\text{ cm}$$

### Key Classes

1. **[`ATetrisBoardActor`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Tetris/TetrisBoardActor.h)**:
   - **Matrix State**: `int32 Grid[20][10]` storing empty (0) or locked piece type colors (1..7).
   - **Tetromino Shapes**: All 7 standard pieces (`I`, `O`, `T`, `S`, `Z`, `J`, `L`) with 4 rotation states defined in a compact 3D offset array.
   - **Rendering**:
     - **Frame**: Left border, right border, bottom border, and dark backplane.
     - **Locked Blocks**: 7 `UInstancedStaticMeshComponent`s (one per color), allowing all 200 blocks to render in single draw calls.
     - **Falling Piece**: 4 active `UStaticMeshComponent`s that follow `(CurrentCol, CurrentRow)`.
     - **Next Piece Preview**: 4 preview blocks displayed in the top right.
   - **Wall Kicks & Collision**: `IsValidPosition()` ensures blocks stay within bounds and don't overlap existing blocks. Basic left/right wall kicks allow rotation even when hugging walls.
   - **Line Clears & Leveling**: Detects full rows, shifts rows above down, awards standard Nintendo-style scoring ($100$, $300$, $500$, $800 \times \text{Level}$), and increases drop speed every 10 lines.

2. **[`ATetrisPawn`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Tetris/TetrisPawn.h)**:
   - Houses an orthographic-perspective camera centered on the $10 \times 20$ board.
   - Binds and routes input to the board.

3. **[`ATetrisGameMode`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Tetris/TetrisGameMode.h)**:
   - Automatically spawns `ATetrisBoardActor` if not present in the level and sets input mode to Game & UI.

4. **[`ATetrisHUD`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Source/LearnUnreal_56_1/Tetris/TetrisHUD.h)**:
   - Renders side statistics (Score, Level, Lines Cleared), Next Piece header, bottom keybind guide, and Game Over screen.

---

## 4. How to Compile & Play in Unreal Engine 5.6

### Step 1: Compile the Code
Because the Unreal Editor is currently open with **Live Coding** active:
1. In the Unreal Editor, press **`Ctrl + Alt + F11`** (or click the **Live Coding** button at the bottom-right corner of the editor).
2. Alternatively, you can close the editor and compile from your IDE (Visual Studio / Rider) or run:
   ```powershell
   & "C:\EpicGames\UE_5.6\Engine\Build\BatchFiles\Build.bat" LearnUnreal_56_1Editor Win64 Development "c:\Unreal Projects\LearnUnreal_56_1\LearnUnreal_56_1.uproject" -waitmutex
   ```

---

### Step 2: How to Play Flappy Bird

1. Open any level in the Editor (e.g., `Content/TopDown/Lvl_TopDown.umap`).
2. Open the **World Settings** tab (**Window** $\rightarrow$ **World Settings** if not visible).
3. Under **GameMode**, set **GameMode Override** to:
   $$\mathbf{FlappyGameMode}$$
4. Click **Play in Editor (PIE)** (`Alt + P`).
5. **Controls**:
   - **`SpaceBar` / `Left Mouse Button` / `Up Arrow` / `W`**: Flap wings to jump and start.
   - **`R` / `SpaceBar`**: Restart after Game Over.

---

### Step 3: How to Play Tetris

1. In the **World Settings** tab, change **GameMode Override** to:
   $$\mathbf{TetrisGameMode}$$
2. Click **Play in Editor (PIE)** (`Alt + P`).
3. **Controls**:
   - **`A` / `Left Arrow`**: Move piece left.
   - **`D` / `Right Arrow`**: Move piece right.
   - **`W` / `Up Arrow`**: Rotate piece clockwise.
   - **`S` / `Down Arrow`**: Soft drop (accelerates fall, $+1\text{ pt}$).
   - **`SpaceBar` / `Enter`**: Hard drop (instant drop & lock, $+2\text{ pts/row}$).
   - **`R`**: Restart game.

---

## 5. Quick Controls Reference Table

| Game | Action | Primary Key | Alternate Keys |
| :--- | :--- | :--- | :--- |
| **Flappy Bird** | Flap / Start | `SpaceBar` | `Left Click`, `Up Arrow`, `W` |
| **Flappy Bird** | Restart | `R` | `SpaceBar` |
| **Tetris** | Move Left | `A` | `Left Arrow` |
| **Tetris** | Move Right | `D` | `Right Arrow` |
| **Tetris** | Rotate | `W` | `Up Arrow` |
| **Tetris** | Soft Drop | `S` | `Down Arrow` |
| **Tetris** | Hard Drop | `SpaceBar` | `Enter` |
| **Tetris** | Restart | `R` | — |

---

Enjoy your Unreal Engine journey Renato!
