# Java Implementations (Flappy Bird & Tetris)

This folder contains standalone, runnable Java implementations of **Flappy Bird** and **Tetris** built using Java's standard GUI library (`Swing` & `AWT`).

**No external dependencies or Gradle/Maven installs required.** Everything compiles and runs cleanly using standard `javac` and `java` (including OpenJDK bundled with Unity Hub).

---

## Folder Structure

```
Games_Java/
├── FlappyBird/
│   ├── src/flappy/FlappyBird.java  # Standalone 60 FPS physics Flappy Bird GUI
│   ├── bin/                        # Compiled .class files
│   └── run.bat                     # Automatic build & launch script
├── Tetris/
│   ├── src/tetris/Tetris.java      # Standalone 10x20 grid Tetris with ghost piece & preview
│   ├── bin/                        # Compiled .class files
│   └── run.bat                     # Automatic build & launch script
└── README.md                       # Documentation and architecture guide
```

---

## How to Run

### Option 1: Double-click or run the batch scripts:
- **Flappy Bird**: Double-click `Games_Java\FlappyBird\run.bat` or run:
  ```powershell
  .\Games_Java\FlappyBird\run.bat
  ```
- **Tetris**: Double-click `Games_Java\Tetris\run.bat` or run:
  ```powershell
  .\Games_Java\Tetris\run.bat
  ```

*(The `run.bat` script automatically checks your system `PATH` as well as the Unity Hub OpenJDK installation at `C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin`).*

### Option 2: Compile & Run directly via terminal:
```powershell
# Flappy Bird:
javac -d Games_Java/FlappyBird/bin Games_Java/FlappyBird/src/flappy/FlappyBird.java
java -cp Games_Java/FlappyBird/bin flappy.FlappyBird

# Tetris:
javac -d Games_Java/Tetris/bin Games_Java/Tetris/src/tetris/Tetris.java
java -cp Games_Java/Tetris/bin tetris.Tetris
```

---

## Key Architectural Differences from Unreal C++, Unity C#, and Python

1. **Double Buffering & Painting Model**:
   - Unlike Unreal (`UInstancedStaticMeshComponent` or `Canvas->DrawText`) or Unity (`MeshRenderer` or `OnGUI`), Java Swing uses a retained paint cycle driven by the Event Dispatch Thread (EDT).
   - Custom rendering is implemented by overriding `protected void paintComponent(Graphics g)`, where `Graphics2D` provides anti-aliasing (`KEY_ANTIALIASING`), affine transformations (`rotate`, `translate`), and hardware double buffering.

2. **Game Loop (`javax.swing.Timer`)**:
   - `javax.swing.Timer` fires an `ActionEvent` every 16 ms (targeting ~60 FPS) directly on the EDT. This eliminates multi-threading synchronization hazards between the physics updates and Swing repaint calls.

3. **Memory & Static Typing**:
   - Statically typed 2D and 4D primitive arrays (`int[][] grid`, `int[][][][] TETROMINO_SHAPES`) provide $O(1)$ block lookups and collision checks without object allocation overhead in the main loop.
   - Fast line clearing is achieved via `System.arraycopy(grid[y - 1], 0, grid[y], 0, GRID_COLS)`.
