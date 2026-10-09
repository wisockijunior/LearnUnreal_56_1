# Unreal Verse Implementations (Flappy Bird & Tetris)

This directory contains complete, idiomatic implementations of **Flappy Bird** and **Tetris** written in **Epic Games' Verse programming language** for Unreal Engine / UEFN (Unreal Editor for Fortnite).

---

## 1. Verse Language Concepts & Architecture

Verse is a modern, statically typed **functional-logic language** designed by Epic Games specifically for real-time metaverses, game simulation, and transactional game state management.

### Key Language Features Illustrated in These Games:

| Feature | Description | Example from Code |
| :--- | :--- | :--- |
| **Failable Contexts (`<decides>`)** | In Verse, operations that might fail (e.g., array out-of-bounds, invalid movement collision) do not throw runtime exceptions or return null. Instead, they use `<decides>` effects and are tested with `if (Expression?)`. | `IsValidPosition(...)<transacts><decides> : void` |
| **Transactional Execution (`<transacts>`)** | Functions marked with `<transacts>` can automatically roll back state changes if an inner failable expression fails. | `Flap()<transacts> : void` |
| **Structured Concurrency (`<suspends>`)** | Async functions can pause execution without blocking the engine's main thread, using `Sleep(DeltaTime)`, `spawn`, `sync`, or `race`. | `GameLoop()<suspends> : void` |
| **Creative Device Model** | Game systems are structured as subclasses of `creative_device` with `@editable` properties connected in the editor. | `flappy_bird_device := class(creative_device)` |
| **Immutable by Default** | Values cannot be modified unless explicitly declared with `var`. | `var Score : int = 0` vs `Gravity : float = 1200.0` |

---

## 2. Folder Structure

```
Games_Verse/
├── FlappyBird/
│   └── flappy_bird_device.verse   # Creative device: async physics loop, dynamic pipes, collisions, score
├── Tetris/
│   └── tetris_device.verse        # Creative device: 10x20 grid, 7 tetrominoes, wall kicks, line clear logic
└── README.md
```

---

## 3. How to Use in UEFN (Unreal Editor for Fortnite)

1. Open your UEFN project.
2. In the top toolbar, click **Verse** $\rightarrow$ **Verse Explorer**.
3. Right-click on your project package $\rightarrow$ **Add new Verse file to project**.
4. Choose **Creative Device** and copy the contents of [`flappy_bird_device.verse`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Verse/FlappyBird/flappy_bird_device.verse) or [`tetris_device.verse`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Verse/Tetris/tetris_device.verse).
5. Click **Verse** $\rightarrow$ **Build Verse Code** (`Ctrl + Shift + B`).
6. Drag the compiled device from the Content Browser into your level viewport.
7. Place standard Fortnite **Button Devices** in the level and link them to the `@editable` slots (`FlapTriggerButton`, `MoveLeftButton`, `RotateButton`, etc.) in the Details panel.
8. Start the session to play!

---

## 4. Architectural Comparison: Verse vs Unreal C++

| Paradigm | Unreal Engine C++ | Unreal Verse |
| :--- | :--- | :--- |
| **Base Class** | `AActor` / `APawn` / `AGameModeBase` | `creative_device` |
| **Main Loop** | Engine-driven `virtual void Tick(float DeltaTime) override` | Asynchronous coroutine `GameLoop()<suspends>` using `Sleep(DeltaTime)` inside `spawn` |
| **Error Handling** | Pointers, booleans, assert macros (`check`, `ensure`) | Mathematical failure (`<decides>`) with automatic rollback |
| **Memory Model** | C++ manual pointers with `UObject*` garbage collection | Managed, safe, immutable value types |
| **Properties** | `UPROPERTY(EditAnywhere, BlueprintReadWrite)` | `@editable PropName : type = default` |
| **Units** | Centimeters ($Z$-Up) | Centimeters ($Z$-Up) |
