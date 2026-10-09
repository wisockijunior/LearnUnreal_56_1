# Folder Structure Overview

```text
LearnUnreal_56_1/
├── Source/LearnUnreal_56_1/         # Original Unreal Engine 5.6 (C++)
│   ├── Flappy/                      # AFlappyBirdPawn, AFlappyPipePair, AFlappyGameMode, AFlappyHUD
│   └── Tetris/                      # ATetrisBoardActor, ATetrisPawn, ATetrisGameMode, ATetrisHUD
│
├── Games_Verse/                     # Unreal Engine / UEFN (Verse Language)
│   ├── FlappyBird/
│   │   └── flappy_bird_device.verse # Creative device: async physics loop, dynamic pipes, collisions
│   ├── Tetris/
│   │   └── tetris_device.verse      # Creative device: 10x20 grid, 7 tetrominoes, wall kicks, line clear
│   └── README.md                    # Verse language guide: <decides>, <transacts>, <suspends>, UEFN setup
│
├── Games_Rust/                      # Rust (Systems, Zero-GC, Memory-Safe)
│   ├── FlappyBird/
│   │   ├── Cargo.toml               # Package configuration & dependencies
│   │   └── src/main.rs              # 60 FPS physics loop, AABB collision, ANSI TUI
│   ├── Tetris/
│   │   ├── Cargo.toml               # Package configuration & dependencies
│   │   └── src/main.rs              # 10x20 grid, 7 tetrominoes, wall kicks, ghost piece, line clear
│   └── README.md                    # Rust guide: ownership/borrowing, zero-cost abstractions, cargo run
│
├── Games_Go/                        # Go / Golang (Zero Dependencies, Concurrency)
│   ├── FlappyBird/
│   │   └── main.go                  # 60 FPS physics loop, goroutines, tickers, ANSI TUI (go run)
│   ├── Tetris/
│   │   └── main.go                  # 10x20 grid, 7 tetrominoes, wall kicks, ghost piece (go run)
│   └── README.md                    # Go guide: goroutines, channels, value semantics, Windows console
│
├── Games_Unity_CSharp/              # Unity3D (C#)
│   ├── FlappyBird/Scripts/
│   │   ├── BirdController.cs        # Flap impulse, gravity, pitch tilt, AABB/colliders
│   │   ├── FlappyPipePair.cs        # Center-pivot offset calculations, score triggers
│   │   ├── FlappySpawner.cs         # Gap height randomization & off-screen cleanup
│   │   ├── FlappyGameManager.cs     # State machine (Ready, Playing, GameOver) & PlayerPrefs
│   │   └── FlappyHUD.cs             # Immediate-mode OnGUI score, title, and game-over banner
│   ├── Tetris/Scripts/
│   │   ├── TetrisBoard.cs           # 10x20 matrix, 7 tetrominoes, wall kicks, line clear, score
│   │   └── TetrisHUD.cs             # OnGUI displaying score, level, lines, next piece preview
│   └── README.md                    # Unity scene setup guide & pivot rules
│
├── Games_Python/                    # Python 3 (Tkinter - Zero Dependencies)
│   ├── FlappyBird/
│   │   └── flappy_bird.py           # Complete 60 FPS physics Flappy Bird GUI
│   ├── Tetris/
│   │   └── tetris.py                # 10x20 grid Tetris with ghost piece & preview
│   └── README.md                    # Python guide & event loop breakdown
│
├── Games_Java/                      # Java 11 (Swing / AWT - Pre-compiled)
│   ├── FlappyBird/
│   │   ├── src/flappy/FlappyBird.java # 60 FPS Swing JPanel with Graphics2D anti-aliasing
│   │   └── run.bat                  # One-click compile & launch script
│   ├── Tetris/
│   │   ├── src/tetris/Tetris.java   # 10x20 grid Tetris with wall-kicks, line clearing
│   │   └── run.bat                  # One-click compile & launch script
│   └── README.md                    # Java guide & EDT / double-buffering breakdown
│
├── Games_JavaME_NokiaE63/           # Nokia E63 J2ME (MIDP 2.0 / CLDC 1.1)
│   ├── FlappyBird/                  # 320x240 landscape Flappy Bird MIDlet
│   ├── Tetris/                      # 320x240 landscape Tetris MIDlet
│   ├── Emulator/                    # Interactive Nokia E63 hardware skin & simulator
│   ├── dist/                        # Ready-to-install FlappyBird.jar and Tetris.jar
│   └── README.md
│
└── ARCHITECTURE_COMPARISON.md       # Master cross-language architectural comparison
```
