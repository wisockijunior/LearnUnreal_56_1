# Python Implementations (Flappy Bird & Tetris)

This folder contains standalone, runnable Python implementations of **Flappy Bird** and **Tetris** built using Python's built-in `tkinter` library.

**No external packages (such as Pygame or SDL) are required.** Both games run immediately using standard Python 3.

---

## Folder Structure

```
Games_Python/
├── FlappyBird/
│   └── flappy_bird.py    # Standalone 60 FPS physics Flappy Bird GUI
├── Tetris/
│   └── tetris.py         # Standalone 10x20 grid Tetris with ghost piece and preview
└── README.md             # Documentation and architectural comparison
```

---

## How to Run

From any terminal (PowerShell, Command Prompt, or bash):

### Run Flappy Bird:
```powershell
python Games_Python\FlappyBird\flappy_bird.py
```
- **Controls**: `Space`, `Up Arrow`, `W`, or **Left Mouse Click** to flap. `R` to restart.

### Run Tetris:
```powershell
python Games_Python\Tetris\tetris.py
```
- **Controls**:
  - `A` / `D` or `Left` / `Right Arrow`: Move Left / Right
  - `W` or `Up Arrow`: Rotate Piece (with wall-kicks)
  - `S` or `Down Arrow`: Soft Drop
  - `Space` or `Enter`: Hard Drop
  - `R`: Restart Game

---

## Key Architectural Differences from Unreal C++ & Unity C#

1. **Game Loop (`root.after` vs `Tick` / `Update`)**:
   - In Unreal and Unity, the engine's native main loop calls `Tick(DeltaTime)` or `Update()` every frame.
   - In Tkinter Python, the application relies on an event-driven loop (`root.mainloop()`). To achieve 60 FPS, a recursive timer callback `root.after(16, self.game_loop)` schedules the next physics update and redraw.

2. **Coordinates & Pivots ($Y$-Down Screen Space)**:
   - **Unreal**: $(0,0,0)$ in 3D world space, $Z$-up, centered pivots, centimeters.
   - **Unity**: $(0,0,0)$ in 3D world space, $Y$-up, centered pivots, meters.
   - **Python Tkinter**: $(0,0)$ is the **Top-Left** corner of the window. $+X$ is Right, $+Y$ is **Down** (pixels).
     - Because $+Y$ is down, gravity is positive (`+0.55`), and flapping applies a negative impulse (`-9.2`).

3. **Data Representation & Garbage Collection**:
   - In C++, tetromino pieces and board cells are statically typed arrays and structs (`int32 Grid[20][10]`).
   - In Python, pieces are nested lists and tuples (`TETROMINO_SHAPES`), and line clearing is done dynamically with list operations (`del grid[r]`, `grid.insert(0, ...)`), handled by Python's automatic garbage collector.
