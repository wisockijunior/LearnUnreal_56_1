# Cross-Language Game Architecture Comparison

This document provides a comprehensive side-by-side architectural and code comparison of **Flappy Bird** and **Tetris** implemented across seven technology stacks:
1. **Unreal Engine 5.6 (C++)** (`Source/LearnUnreal_56_1/`)
2. **Unreal Verse (UEFN)** (`Games_Verse/`)
3. **Rust** (`Games_Rust/`)
4. **Go (Golang)** (`Games_Go/`)
5. **Unity3D (C#)** (`Games_Unity_CSharp/`)
6. **Python (Tkinter)** (`Games_Python/`)
7. **Java (Swing & Nokia E63 J2ME)** (`Games_Java/` & `Games_JavaME_NokiaE63/`)

---

## 1. High-Level Paradigm & Architecture

| Feature | Unreal Engine 5.6 (C++) | Unreal Verse (UEFN) | Rust | Go (Golang) | Unity3D (C#) | Python (Tkinter) | Java (Swing) |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Paradigm** | Actor-Component & GameMode Framework | Functional-Logic (Transactional) | Systems (Ownership / Borrowing) | Procedural & CSP Concurrency | Entity-Component-System (`MonoBehaviour`) | Scripting with Event-Driven GUI | OOP with Event Dispatch Thread |
| **Language** | C++17 / C++20 | Verse (Epic Games) | Rust (2021 Edition) | Go 1.26 | C# 9 / 10 | Python 3 | Java 11+ / J2ME MIDP 2.0 |
| **Memory Model** | UObject GC + Manual RAII | Managed engine value types | Strict Borrow Checker (Zero GC) | Concurrent Tri-Color GC | .NET Garbage Collector | Reference Counting + Cyclic GC | JVM Generational GC / RMS |
| **Game Loop** | Native `Tick(float DeltaTime)` | Coroutine `GameLoop()<suspends>` | Deterministic `Instant::now()` loop | Concurrent `time.NewTicker` | Native `Update()` / `FixedUpdate()` | `root.after(16, loop)` | `javax.swing.Timer` (16ms) |
| **Dependencies** | Unreal 5.6 Editor & UBT | UEFN Verse Compiler | Cargo (`crossterm`) | **None** (Standard Library) | Unity Editor | **None** (Standard Library) | **None** (Standard JDK) |


---

## 2. Coordinate Systems, Units & Pivots

A frequent source of bugs when porting between game engines and desktop frameworks is coordinate systems and pivot rules:

```
Unreal Engine (Left-Handed, Z-Up)
      +Z (Up)
       |   +X (Forward)
       |  /
       | /
       +-------- +Y (Right)

Unity3D (Left-Handed, Y-Up)
      +Y (Up)
       |   +Z (Forward)
       |  /
       | /
       +-------- +X (Right)

Python Tkinter & Java Swing (2D Screen Coordinates, Y-Down)
(0,0) +-------------------> +X (Right)
      |
      |
      v +Y (Down / Floor)
```

### Detailed Coordinate Rules

| Engine / Language | Forward Axis | Lateral Axis | Vertical Axis | Default Units | Primitive Pivot Rule |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Unreal Engine C++** | $+X$ | $+Y$ (Right) | $+Z$ (Up) | Centimeters ($100\text{ cm} = 1\text{ m}$) | Center of mesh geometry |
| **Unity3D C#** | $+Z$ | $+X$ (Right) | $+Y$ (Up) | Meters ($1\text{ unit} = 1\text{ m}$) | **Center `(0, 0, 0)`** of primitive |
| **Python (Tkinter)** | N/A (2D) | $+X$ (Right) | $+Y$ (**Down**) | Screen Pixels | Top-Left for rects, Center/Bounding-box for ovals |
| **Java (Swing)** | N/A (2D) | $+X$ (Right) | $+Y$ (**Down**) | Screen Pixels | Top-Left `(x, y)` for bounding rectangles |

### Critical Unity Y-Coordinate & Pivot Check
In Unity, standard 3D primitives (`Cube`, `Cylinder`) have their **pivot located at the geometric center `(0, 0, 0)`**.
- A cylinder or cube of height $H$ extends from $-H/2$ to $+H/2$ along its local $Y$ axis.
- In `FlappyPipePair.cs`:
  ```csharp
  // Top pipe center calculation (accounting for center pivot):
  topPipeTransform.localPosition = new Vector3(0.0f, gapCenterY + halfGap + halfPipe, 0.0f);

  // Bottom pipe center calculation:
  bottomPipeTransform.localPosition = new Vector3(0.0f, gapCenterY - halfGap - halfPipe, 0.0f);
  ```
- In Python / Java:
  Because $Y$ increases **downwards**, the top pipe extends from $y = 0$ down to $y = \text{gapCenterY} - \text{halfGap}$, and the bottom pipe extends from $y = \text{gapCenterY} + \text{halfGap}$ down to the floor.

---

## 3. Game Loop & Physics Lifecycle

### Flappy Bird: Physics Integration

#### Unreal Engine C++ (`FlappyBirdPawn.cpp`):
```cpp
void AFlappyBirdPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsDead) return;

    // Acceleration
    VerticalVelocity -= Gravity * DeltaTime;

    // Movement along Z
    FVector Loc = GetActorLocation();
    Loc.Z += VerticalVelocity * DeltaTime;
    SetActorLocation(Loc);

    // Dynamic pitch tilt
    float TargetPitch = FMath::Clamp(VerticalVelocity * 0.08f, -75.0f, 25.0f);
    SetActorRotation(FRotator(TargetPitch, 0.0f, 0.0f));
}
```

#### Unity C# (`BirdController.cs`):
```csharp
void Update()
{
    if (isDead) return;

    // Acceleration
    verticalVelocity -= gravity * Time.deltaTime;

    // Movement along Y
    Vector3 pos = transform.position;
    pos.y += verticalVelocity * Time.deltaTime;
    transform.position = pos;

    // Dynamic pitch tilt around Z
    float targetPitch = verticalVelocity > 0.0f
        ? Mathf.Lerp(0.0f, maxUpPitch, verticalVelocity / flapStrength)
        : Mathf.Lerp(0.0f, maxDownPitch, -verticalVelocity / (flapStrength * 1.5f));
    transform.rotation = Quaternion.Euler(0.0f, 0.0f, targetPitch);
}
```

#### Python (`flappy_bird.py`):
```python
def update_simulation(self):
    # In Tkinter, +Y is downward, so gravity is positive!
    self.bird_vy += GRAVITY
    if self.bird_vy > MAX_FALL_SPEED:
        self.bird_vy = MAX_FALL_SPEED

    self.bird_y += self.bird_vy

    # Flap applies negative velocity:
    # def flap(self): self.bird_vy = -9.2
```

#### Java (`FlappyBird.java`):
```java
// Swing Timer (16ms)
birdVy += GRAVITY;
if (birdVy > MAX_FALL_SPEED) birdVy = MAX_FALL_SPEED;
birdY += birdVy;

// In paintComponent: AffineTransform rotates bird based on birdVy
AffineTransform old = g2.getTransform();
g2.translate(BIRD_X, (int) birdY);
g2.rotate(birdVy > 0 ? Math.min(Math.toRadians(70), birdVy * 0.08) : Math.max(Math.toRadians(-25), birdVy * 0.08));
// ... render bird ...
g2.setTransform(old);
```

---

## 4. Tetris: Grid Representation & Matrix Operations

### Data Structures

| Implementation | Grid Representation | Shape Data Format | Line Clear Technique |
| :--- | :--- | :--- | :--- |
| **Unreal C++** | `int32 Grid[20][10]` | Hardcoded static 4D array `[7][4][4][2]` | In-place row copy loop with `FMemory::Memcpy` or row swap |
| **Unity C#** | `int[,] grid = new int[20, 10]` | `int[,,,] TETROMINO_SHAPES` | Multi-dimensional array iteration shifting rows downward |
| **Python** | List of lists `[[0]*10 for _ in range(20)]` | List of lists of coordinate tuples | Dynamic slice deletion: `del grid[r]` + `grid.insert(0, [0]*10)` |
| **Java** | `int[][] grid = new int[20][10]` | `int[][][][] TETROMINO_SHAPES` | High-speed memory copy: `System.arraycopy(grid[y-1], 0, grid[y], 0, 10)` |

### Rotation & Wall Kicks

All four implementations implement standard Tetris rotation states with wall kicks:
1. Attempt basic clockwise rotation `(rot + 1) % 4`.
2. If blocked by the wall or locked blocks, attempt offset checks (Kick Left $x-1$, Kick Right $x+1$, or Floor Kick $y-1$).
3. If all offset positions collide, the rotation is cancelled.

---

## 5. Rendering Pipeline Comparison

### Unreal Engine 5.6
- **Playfield Blocks**: Rendered via `UInstancedStaticMeshComponent` (ISMs). This collapses hundreds of individual block draw calls down to 1 batched draw call per piece color, keeping frame times under 0.2ms.
- **HUD**: `AFlappyHUD` / `ATetrisHUD` uses direct C++ canvas drawing via `UCanvas::DrawText` with shadow passes.

### Unity3D
- **Playfield Blocks**: Rendered via primitive `Cube` GameObjects under a common `BoardVisualRoot`.
- **HUD**: `OnGUI()` provides zero-dependency, immediate-mode GUI labels and boxes without requiring Canvas/uGUI asset bundles.

### Python Tkinter
- **Playfield & HUD**: Single `tk.Canvas` widget.
- **Frame Cycle**: Every 16ms, `self.canvas.delete("all")` is called, and primitives (`create_rectangle`, `create_oval`, `create_text`) are drawn in retained-mode display lists.

### Java Swing / AWT
- **Playfield & HUD**: Custom `JPanel` with overridden `protected void paintComponent(Graphics g)`.
- **Double Buffering**: Automatically handled by Swing. `Graphics2D` handles anti-aliased geometry, affine transforms, beveled block borders, and text metrics.

---

## 6. Rosetta Stone: Key Engine & Language Concepts

| Feature / Concept | Unreal Engine (C++) | Unity3D (C#) | Python (Tkinter) | Java (Swing) |
| :--- | :--- | :--- | :--- | :--- |
| **Base Entity** | `AActor` | `GameObject` / `MonoBehaviour` | `object` / Class instance | `Object` / `JPanel` |
| **Delta Time** | `DeltaTime` in `Tick()` | `Time.deltaTime` in `Update()` | Calculated from timestamp diff | Fixed interval timer (16ms) |
| **Dynamic Array** | `TArray<T>` | `List<T>` | `list` | `ArrayList<T>` |
| **String Type** | `FString` / `FText` / `FName` | `string` | `str` | `String` |
| **Logging** | `UE_LOG(LogTemp, Warning, ...)` | `Debug.Log(...)` | `print(...)` | `System.out.println(...)` |
| **Math Library** | `FMath` / `FVector` | `Mathf` / `Vector3` | `math` module | `java.lang.Math` |
| **Input Handling** | `PlayerInputComponent->BindAction` | `Input.GetKeyDown(KeyCode)` | `root.bind("<key>", callback)` | `KeyListener` / `KeyAdapter` |
| **High Score Persistence** | `USaveGame` | `PlayerPrefs` | `json` / file I/O | `Preferences` / file I/O |

---

## 7. How to Run Each Implementation

```bash
# 1. Unreal Engine 5.6 (C++):
# Open LearnUnreal_56_1.uproject in Unreal Editor, open FlappyBirdMap or TetrisMap, click Play.

# 2. Unity3D (C#):
# Drag Games_Unity_CSharp into any Unity Assets folder, open Flappy or Tetris scene, click Play.

# 3. Python (Tkinter):
python Games_Python/FlappyBird/flappy_bird.py
python Games_Python/Tetris/tetris.py

# 4. Java (Swing):
./Games_Java/FlappyBird/run.bat
./Games_Java/Tetris/run.bat
```
