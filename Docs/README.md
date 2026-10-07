# Project Documentation

Welcome to the **LearnUnreal_56_1** project documentation!

This repository includes two games implemented entirely in C++ for Unreal Engine 5.6: **Flappy Bird** and **Tetris**, specifically written with architectural notes for senior Unity developers transitioning to Unreal Engine.

## Documentation Index

- [**Folder Structure Overview**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/Folder%20Structure%20Overview.md)
  - Quick, clean ASCII tree overview of all game implementations (Unreal C++, Unity C#, Python, Java Swing, and Nokia E63 J2ME).
- [**Workspace Folder Structure & Architecture Reference**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/Folder%20Structure.md)
  - Complete repository tree mapping root files, Unreal Engine 5.6 C++ source, Content assets, and cross-platform implementations.
- [**C++ Level Classes & Blueprint Breakdown**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/Folder%20Structure%20Summary.md)
  - Focused breakdown of all C++ folders (`TopDown`, `Variant_Strategy`, `Variant_TwinStick`, `Flappy`, `Tetris`), class responsibilities, and mapping to Content Blueprint subclasses.
- [**Scene Setup & Gameplay Guide**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/SCENE_SETUP_AND_PLAY_GUIDE.md)
  - Step-by-step instructions on setting up levels (`.umap`), configuring lighting, setting GameMode overrides in World Settings, running in PIE, and tweaking gameplay values.
- [**Tetris Hard Drop Fix (Post-Mortem)**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Docs/TETRIS_HARD_DROP_FIX.md)
  - Why one Space press dropped two pieces, why the fixes seemed not to work (a stale binary after failed Live Coding builds), and the 3-layer input fix (single input source, debounce, release-gate).
- [**Unity Developer to Unreal C++ Rosetta Stone**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/TUTORIAL_UNITY_TO_UNREAL.md)
  - Full conceptual comparison between Unity and Unreal (`GameObject` $\leftrightarrow$ `AActor`, `MonoBehaviour` $\leftrightarrow$ `UActorComponent`, `Update` $\leftrightarrow$ `Tick`, etc.), coordinate system specifics (Centimeters, $Z$-Up), and code breakdowns.
- [**Cross-Language Architecture Comparison Guide**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/ARCHITECTURE_COMPARISON.md)
  - Side-by-side comparison of **Flappy Bird** and **Tetris** implemented across **Unreal C++**, **Unity C#**, **Python (Tkinter)**, and **Java (Swing)**, including coordinate conversions, pivot checks, memory management, and game loop lifecycles.
- [**Unity3D C# Implementations**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Unity_CSharp/README.md)
  - Ready-to-import Unity C# scripts for both games with explicit center-pivot offset calculations.
- [**Python Implementations**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Python/README.md)
  - Standalone, zero-dependency 60 FPS playable games in standard Python Tkinter.
- [**Java (Swing) Desktop Implementations**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_Java/README.md)
  - Standalone, pre-compiled Java Swing games with automatic build & run scripts.
- [**Nokia E63 J2ME (MIDP 2.0 / CLDC 1.1) Implementations**](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_JavaME_NokiaE63/README.md)
  - Authentic 320x240 landscape J2ME games for Nokia E63 (Symbian S60 3rd Ed.), packaged `.jar` / `.jad` files, and interactive desktop hardware simulator.

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
