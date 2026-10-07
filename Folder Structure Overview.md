# Folder Structure Overview

```text
LearnUnreal_56_1/
├── Source/LearnUnreal_56_1/         # Original Unreal Engine 5.6 (C++)
│   ├── Flappy/                      # AFlappyBirdPawn, AFlappyPipePair, AFlappyGameMode, AFlappyHUD
│   └── Tetris/                      # ATetrisBoardActor, ATetrisPawn, ATetrisGameMode, ATetrisHUD
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
