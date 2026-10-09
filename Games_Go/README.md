# Go (Golang) Implementations (Flappy Bird & Tetris)

This directory contains standalone, zero-dependency implementations of **Flappy Bird** and **Tetris** written in **Go (Golang)** using standard library concurrency (`goroutines`, `time.NewTicker`), Windows console integration, and ANSI terminal rendering.

---

## 1. Go Language Architecture & Key Concepts

Go combines systems-level execution speed with garbage-collected productivity and built-in concurrency primitives:

| Concept | How It's Used in These Implementations |
| :--- | :--- |
| **Goroutines & Tickers** | `time.NewTicker(16 * time.Millisecond)` drives deterministic 60 FPS physics without manual thread management. |
| **Value Semantics & Arrays** | Fixed 2D arrays (`[20][10]int`) and structs are allocated contiguously in memory for cache-friendly matrix operations. |
| **Zero External Dependencies** | Uses Go's built-in `syscall.NewLazyDLL("msvcrt.dll")` to read non-blocking console key presses (`_kbhit` and `_getch`) on Windows. |
| **Garbage Collection** | Automatic concurrent tri-color mark-and-sweep GC with sub-millisecond pauses. |

---

## 2. Folder Structure

```
Games_Go/
├── FlappyBird/
│   └── main.go                    # 60 FPS physics loop, gravity descent, AABB collision, ANSI TUI
├── Tetris/
│   └── main.go                    # 10x20 matrix, 7 tetrominoes, wall kicks, ghost piece, lines clear
└── README.md
```

---

## 3. How to Run

Because Go is installed on your system, you can run them immediately from any terminal:

### 1. Run Flappy Bird:
```powershell
go run Games_Go\FlappyBird\main.go
```
- **Controls**:
  - `Space` / `Up Arrow` / `W`: Flap / Jump
  - `R`: Restart
  - `Q`: Quit

### 2. Run Tetris:
```powershell
go run Games_Go\Tetris\main.go
```
- **Controls**:
  - `A` / `D` or `Left` / `Right Arrow`: Move Left / Right
  - `W` or `Up Arrow`: Rotate Clockwise
  - `S` or `Down Arrow`: Soft Drop
  - `Space` or `Enter`: Hard Drop
  - `R`: Restart
  - `Q`: Quit

---

## 4. Architectural Comparison: Go vs Rust vs Verse vs Unreal C++

| Metric | Unreal Engine C++ | Unreal Verse | Pure Rust | Go (Golang) |
| :--- | :--- | :--- | :--- | :--- |
| **Paradigm** | Object-Oriented (Actor-Component) | Functional-Logic (Transactional) | Systems (Ownership/Borrowing) | Procedural / Concurrent |
| **Memory Model** | UObject GC + Manual C++ RAII | Value types & managed engine memory | Strict Borrow Checker (Zero GC) | Concurrent Tri-Color GC |
| **Concurrency** | Threads, Async Tasks, Tick | `<suspends>`, `spawn`, `Sleep`, `sync` | Threads, `Send`/`Sync`, Channels | Goroutines, Channels, `select` |
| **Execution** | Native Machine Code (MSVC/Clang) | Bytecode on UEFN Verse VM | Native Machine Code (LLVM) | Native Machine Code (Go compiler) |
