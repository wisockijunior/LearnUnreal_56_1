# Unity3D C# Implementations (Flappy Bird & Tetris)

This folder contains clean, self-contained Unity C# implementations of **Flappy Bird** and **Tetris**, designed as direct architectural counterparts to the Unreal Engine C++ implementations in `Source/LearnUnreal_56_1/`.

---

## Folder Structure

```
Games_Unity_CSharp/
├── FlappyBird/
│   └── Scripts/
│       ├── BirdController.cs     # Jump physics, gravity, pitch tilt, collisions (AFlappyBirdPawn)
│       ├── FlappyPipePair.cs     # Dynamic pipe height, center-pivot offsets, score trigger (AFlappyPipePair)
│       ├── FlappySpawner.cs      # Randomized gap spawner and off-screen cleanup
│       ├── FlappyGameManager.cs  # State machine (Ready, Playing, GameOver), PlayerPrefs high score (AFlappyGameMode)
│       └── FlappyHUD.cs          # Immediate-mode OnGUI score, title, and game-over banner (AFlappyHUD)
└── Tetris/
    └── Scripts/
        ├── TetrisBoard.cs        # 10x20 matrix, 7 tetrominoes, wall kicks, line clear, scoring (ATetrisBoardActor)
        └── TetrisHUD.cs          # Real-time HUD with score, lines, level, controls, game over (ATetrisHUD)
```

---

## Unity Y-Coordinate & Pivot Check (Best Practice Note)

In Unity, default 3D primitives (`Cube`, `Cylinder`) have their **pivot located at the geometric center `(0, 0, 0)`**.
- A cylinder or cube of height $H$ extends from $-H/2$ to $+H/2$ along its local $Y$ axis.
- **Flappy Bird Pipes (`FlappyPipePair.cs`)**:
  - The top pipe's bottom opening must align with `GapCenterY + (GapSize / 2)`.
  - Because the pivot is in the center, its transform local position must be:
    $$\text{Local Y} = \text{GapCenterY} + \frac{\text{GapSize}}{2} + \frac{\text{PipeLength}}{2}$$
  - The bottom pipe's top opening must align with `GapCenterY - (GapSize / 2)`:
    $$\text{Local Y} = \text{GapCenterY} - \frac{\text{GapSize}}{2} - \frac{\text{PipeLength}}{2}$$
  This avoids mesh clipping, floating pipes, or mismatched gap colliders.

---

## How to Test in Unity

### 1. Flappy Bird Setup:
1. Create a new empty Unity 3D or 2D scene.
2. Create an empty GameObject named `GameManager` and attach `FlappyGameManager`, `FlappySpawner`, and `FlappyHUD`.
3. Create a 3D Sphere at `(0, 0, 0)` named `Bird`, tag it `Player`, add a `SphereCollider`, and attach `BirdController`.
4. Position the Main Camera at `(0, 0, -10)` looking straight forward at `(0, 0, 0)` with Orthographic projection or Perspective ($FOV = 60$).
5. Hit **Play**! Press `Space` or click to flap.

### 2. Tetris Setup:
1. Create a new empty Unity scene.
2. Create an empty GameObject named `TetrisManager` at `(0, 0, 0)`.
3. Attach `TetrisBoard` and `TetrisHUD`.
4. Position the Main Camera at `(0, 0, -22)` looking at `(0, 0, 0)`.
5. Hit **Play**! Use `A`/`D` to move, `W` to rotate, `S` for soft drop, and `Space` for hard drop.
