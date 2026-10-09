# Rust Implementations (Flappy Bird & Tetris)

This directory contains standalone, memory-safe, zero-GC implementations of **Flappy Bird** and **Tetris** written in **Rust** using the cross-platform terminal library `crossterm`.

---

## 1. Rust Language Architecture & Key Concepts

Rust delivers systems-level C++ performance with compiler-guaranteed memory and thread safety without requiring a garbage collector:

| Concept | How It's Used in These Implementations |
| :--- | :--- |
| **Ownership & Borrowing** | Board states and pipe vectors are owned by the `Game` struct; functions borrow references (`&self`, `&mut self`), preventing data races and memory leaks. |
| **Pattern Matching (`match`)** | State machines (`GameState::Ready`, `GameState::Playing`, `GameState::GameOver`) and keyboard events (`KeyCode`) are processed safely with exhaustive pattern matching. |
| **Zero-Cost Abstractions** | Arrays (`[[u8; 10]; 20]`) and iterators (`iter().all(...)`) compile down to vectorized assembly without runtime overhead. |
| **No Garbage Collection** | Memory is allocated on the stack and freed deterministically via RAII when variables exit scope. |

---

## 2. Folder Structure

```
Games_Rust/
├── FlappyBird/
│   ├── Cargo.toml                 # Package configuration & dependencies
│   └── src/main.rs                # 60 FPS physics loop, gravity descent, AABB collision, ANSI TUI
├── Tetris/
│   ├── Cargo.toml                 # Package configuration & dependencies
│   └── src/main.rs                # 10x20 matrix, 7 tetrominoes, wall kicks, ghost piece, lines clear
└── README.md
```

---

## 3. How to Run

To run on any system with Rust and Cargo installed:

### 1. Run Flappy Bird:
```bash
cd Games_Rust/FlappyBird
cargo run
```
- **Controls**:
  - `Space` / `Up Arrow` / `W`: Flap / Jump
  - `R`: Restart
  - `Q`: Quit

### 2. Run Tetris:
```bash
cd Games_Rust/Tetris
cargo run
```
- **Controls**:
  - `A` / `D` or `Left` / `Right Arrow`: Move Left / Right
  - `W` or `Up Arrow`: Rotate Clockwise
  - `S` or `Down Arrow`: Soft Drop
  - `Space` or `Enter`: Hard Drop
  - `R`: Restart
  - `Q`: Quit

---

## 4. Architectural Comparison: Rust vs Unreal C++

| Metric | Unreal Engine 5.6 C++ | Pure Rust |
| :--- | :--- | :--- |
| **Memory Management** | Manual pointers + `UObject` Garbage Collector | Strict compile-time borrow checker + RAII (Zero GC) |
| **Game Loop** | Engine-driven `virtual void Tick(float DeltaTime)` | Custom deterministic loop with `Instant::now()` and `thread::sleep` |
| **Error Handling** | Pointers / nullptr checks / assertions | `Option<T>` and `Result<T, E>` algebraic data types |
| **Concurrency** | Threads, Tasks (`FTaskGraphInterface`) | Fearless concurrency (`Send`, `Sync`, channels) |
