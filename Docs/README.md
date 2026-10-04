# Project Documentation

Welcome to the **LearnUnreal_56_1** project documentation!

This repository includes two games implemented entirely in C++ for Unreal Engine 5.6: **Flappy Bird** and **Tetris**, specifically written with architectural notes for senior Unity developers transitioning to Unreal Engine.

## Documentation Index

- [**Scene Setup & Gameplay Guide**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/SCENE_SETUP_AND_PLAY_GUIDE.md)
  - Detailed step-by-step instructions on setting up levels (`.umap`), configuring lighting, setting GameMode overrides in World Settings, running in PIE, tweaking gameplay values, and troubleshooting.
- [**Unity Developer to Unreal C++ Rosetta Stone & Architecture Guide**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/TUTORIAL_UNITY_TO_UNREAL.md)
  - Full conceptual comparison between Unity and Unreal (`GameObject` $\leftrightarrow$ `AActor`, `MonoBehaviour` $\leftrightarrow$ `UActorComponent`, `Update` $\leftrightarrow$ `Tick`, etc.), coordinate system specifics (Centimeters, $Z$-Up), and code breakdowns for both games.

---

## Quick Reference: Controls

### Flappy Bird
| Action | Key |
| :--- | :--- |
| **Flap / Start Game** | `SpaceBar`, `Left Mouse Button`, `Up Arrow`, `W` |
| **Restart** | `R`, `SpaceBar` |

### Tetris
| Action | Key |
| :--- | :--- |
| **Move Left / Right** | `A` / `D` or `Left Arrow` / `Right Arrow` |
| **Rotate Clockwise** | `W` or `Up Arrow` |
| **Soft Drop** | `S` or `Down Arrow` |
| **Hard Drop** | `SpaceBar` or `Enter` |
| **Restart Game** | `R` |
