# Copyright Renato / LearnUnreal_56_1. All Rights Reserved.
# Tetris in Unreal Verse (UEFN / Verse Language)
# Demonstrates Verse 10x20 grid matrices, failable indexing (<decides>),
# transactional rollback (<transacts>), and button event bindings.

using { /Fortnite.com/Devices }
using { /Verse.org/Simulation }
using { /UnrealEngine.com/Temporary/Diagnostics }
using { /UnrealEngine.com/Temporary/SpatialMath }

# 2D relative offset for tetromino blocks
block_offset := struct<concrete>:
    dCol : int
    dRow : int

# Creative Device controlling the Tetris gameplay simulation
tetris_device := class(creative_device):

    # Editable input triggers for UEFN button devices
    @editable MoveLeftButton : button_device = button_device{}
    @editable MoveRightButton : button_device = button_device{}
    @editable RotateButton : button_device = button_device{}
    @editable SoftDropButton : button_device = button_device{}
    @editable HardDropButton : button_device = button_device{}
    @editable RestartButton : button_device = button_device{}

    # Grid Constants
    GridCols : int = 10
    GridRows : int = 20

    # Board state: 1D array of 200 integers (0 = empty, 1..7 = piece color)
    var BoardGrid : []int = array{}

    # Active piece state
    var CurrentPiece : int = 0
    var CurrentRotation : int = 0
    var CurrentCol : int = 3
    var CurrentRow : int = 19
    var NextPiece : int = 1

    # Progression state
    var Score : int = 0
    var LinesCleared : int = 0
    var Level : int = 1
    var IsGameOver : logic = false
    var DropIntervalSeconds : float = 0.8
    var TimeSinceLastDrop : float = 0.0

    # Lifecycle start
    OnBegin<override>()<suspends> : void =
        Print("Verse Tetris Device Initialized.")

        # Bind button interactions
        MoveLeftButton.InteractedWithEvent.Subscribe(OnMoveLeftPressed)
        MoveRightButton.InteractedWithEvent.Subscribe(OnMoveRightPressed)
        RotateButton.InteractedWithEvent.Subscribe(OnRotatePressed)
        SoftDropButton.InteractedWithEvent.Subscribe(OnSoftDropPressed)
        HardDropButton.InteractedWithEvent.Subscribe(OnHardDropPressed)
        RestartButton.InteractedWithEvent.Subscribe(OnRestartPressed)

        RestartGame()

        # Launch async drop timer loop
        spawn:
            GameLoop()

    # Initialize empty 10x20 board (200 zeroes)
    RestartGame()<transacts> : void =
        var InitialGrid : []int = array{}
        for (Index := 0..199):
            set InitialGrid = InitialGrid + array{0}
        set BoardGrid = InitialGrid

        set Score = 0
        set LinesCleared = 0
        set Level = 1
        set IsGameOver = false
        set DropIntervalSeconds = 0.8
        set TimeSinceLastDrop = 0.0
        set CurrentPiece = 0
        set NextPiece = 1
        SpawnNewPiece()
        Print("New Tetris Game Started! Level: {Level}")

    # Core async gravity drop loop
    GameLoop()<suspends> : void =
        TickRate : float = 0.033
        loop:
            Sleep(TickRate)
            if (IsGameOver = false):
                set TimeSinceLastDrop = TimeSinceLastDrop + TickRate
                if (TimeSinceLastDrop >= DropIntervalSeconds):
                    set TimeSinceLastDrop = 0.0
                    StepDrop()

    # Move Left
    OnMoveLeftPressed(Agent : agent) : void =
        if (IsGameOver = false):
            if (IsValidPosition(CurrentPiece, CurrentRotation, CurrentCol - 1, CurrentRow)?):
                set CurrentCol = CurrentCol - 1

    # Move Right
    OnMoveRightPressed(Agent : agent) : void =
        if (IsGameOver = false):
            if (IsValidPosition(CurrentPiece, CurrentRotation, CurrentCol + 1, CurrentRow)?):
                set CurrentCol = CurrentCol + 1

    # Rotate with basic wall-kick checks
    OnRotatePressed(Agent : agent) : void =
        if (IsGameOver = false):
            NextRot := (CurrentRotation + 1) - (4 * ((CurrentRotation + 1) / 4)) # Mod 4
            if (IsValidPosition(CurrentPiece, NextRot, CurrentCol, CurrentRow)?):
                set CurrentRotation = NextRot
            else if (IsValidPosition(CurrentPiece, NextRot, CurrentCol - 1, CurrentRow)?):
                set CurrentCol = CurrentCol - 1
                set CurrentRotation = NextRot
            else if (IsValidPosition(CurrentPiece, NextRot, CurrentCol + 1, CurrentRow)?):
                set CurrentCol = CurrentCol + 1
                set CurrentRotation = NextRot

    # Soft Drop
    OnSoftDropPressed(Agent : agent) : void =
        if (IsGameOver = false):
            if (IsValidPosition(CurrentPiece, CurrentRotation, CurrentCol, CurrentRow - 1)?):
                set CurrentRow = CurrentRow - 1
                set Score = Score + 1
                set TimeSinceLastDrop = 0.0
            else:
                LockPiece()

    # Hard Drop
    OnHardDropPressed(Agent : agent) : void =
        if (IsGameOver = false):
            var DropDistance : int = 0
            loop:
                if (IsValidPosition(CurrentPiece, CurrentRotation, CurrentCol, CurrentRow - 1)?):
                    set CurrentRow = CurrentRow - 1
                    set DropDistance = DropDistance + 1
                else:
                    break
            set Score = Score + (DropDistance * 2)
            LockPiece()

    # Restart button
    OnRestartPressed(Agent : agent) : void =
        RestartGame()

    # Step Drop
    StepDrop()<transacts> : void =
        if (IsValidPosition(CurrentPiece, CurrentRotation, CurrentCol, CurrentRow - 1)?):
            set CurrentRow = CurrentRow - 1
        else:
            LockPiece()

    # Lock active piece into grid
    LockPiece()<transacts> : void =
        Offsets := GetPieceOffsets(CurrentPiece, CurrentRotation)
        for (Offset : Offsets):
            C := CurrentCol + Offset.dCol
            R := CurrentRow - Offset.dRow
            if (C >= 0 and C < GridCols and R >= 0 and R < GridRows):
                Index := (R * GridCols) + C
                if (UpdatedGrid := BoardGrid.Insert[Index, array{CurrentPiece + 1}]):
                    # Replace cell in Verse array
                    set BoardGrid = UpdatedGrid

        CheckLineClears()
        SpawnNewPiece()

    # Line Clears and Level Progression
    CheckLineClears()<transacts> : void =
        var ClearedThisTurn : int = 0
        var NewGrid : []int = array{}

        for (R := 0..GridRows - 1):
            var RowFull : logic = true
            for (C := 0..GridCols - 1):
                Index := (R * GridCols) + C
                if (Cell := BoardGrid[Index]):
                    if (Cell = 0):
                        set RowFull = false

            if (RowFull = true):
                set ClearedThisTurn = ClearedThisTurn + 1
            else:
                # Keep row
                for (C := 0..GridCols - 1):
                    Index := (R * GridCols) + C
                    if (Cell := BoardGrid[Index]):
                        set NewGrid = NewGrid + array{Cell}

        # Add empty rows at top for cleared lines
        for (Pad := 0..ClearedThisTurn - 1):
            for (C := 0..GridCols - 1):
                set NewGrid = NewGrid + array{0}

        set BoardGrid = NewGrid

        if (ClearedThisTurn > 0):
            set LinesCleared = LinesCleared + ClearedThisTurn
            # Classic scoring: 100, 300, 500, 800
            var Points : int = 0
            if (ClearedThisTurn = 1): set Points = 100 * Level
            if (ClearedThisTurn = 2): set Points = 300 * Level
            if (ClearedThisTurn = 3): set Points = 500 * Level
            if (ClearedThisTurn >= 4): set Points = 800 * Level
            set Score = Score + Points

            # Level progression
            set Level = 1 + (LinesCleared / 10)
            if (Level > 1):
                set DropIntervalSeconds = 0.8 * (1.0 - (0.05 * Level))
                if (DropIntervalSeconds < 0.1):
                    set DropIntervalSeconds = 0.1

            Print("Cleared {ClearedThisTurn} lines! Score: {Score}, Level: {Level}")

    # Spawn new piece
    SpawnNewPiece()<transacts> : void =
        set CurrentPiece = NextPiece
        # Pseudo-random next piece (0..6)
        set NextPiece = (Score + LinesCleared + 3) - (7 * ((Score + LinesCleared + 3) / 7))
        set CurrentRotation = 0
        set CurrentCol = 3
        set CurrentRow = 18

        if (not IsValidPosition(CurrentPiece, CurrentRotation, CurrentCol, CurrentRow)?):
            set IsGameOver = true
            Print("Game Over! Final Score: {Score}")

    # Failable position validity test in Verse (<decides>)
    IsValidPosition(Piece : int, Rot : int, Col : int, Row : int)<transacts><decides> : void =
        Offsets := GetPieceOffsets(Piece, Rot)
        for (Offset : Offsets):
            C := Col + Offset.dCol
            R := Row - Offset.dRow
            # Boundary checks (failing condition in Verse)
            if (C < 0 or C >= GridCols or R < 0 or R >= GridRows):
                false?
            # Cell occupancy check
            Index := (R * GridCols) + C
            Cell := BoardGrid[Index]
            if (Cell <> 0):
                false?

    # Return 4 block offsets for any piece and rotation
    GetPieceOffsets(Piece : int, Rot : int) : []block_offset =
        case (Piece):
            # 0: I Piece
            0 =>
                case (Rot):
                    1 => array{block_offset{dCol:=2, dRow:=0}, block_offset{dCol:=2, dRow:=1}, block_offset{dCol:=2, dRow:=2}, block_offset{dCol:=2, dRow:=3}}
                    _ => array{block_offset{dCol:=0, dRow:=1}, block_offset{dCol:=1, dRow:=1}, block_offset{dCol:=2, dRow:=1}, block_offset{dCol:=3, dRow:=1}}
            # 1: O Piece
            1 => array{block_offset{dCol:=1, dRow:=0}, block_offset{dCol:=2, dRow:=0}, block_offset{dCol:=1, dRow:=1}, block_offset{dCol:=2, dRow:=1}}
            # 2: T Piece
            2 => array{block_offset{dCol:=1, dRow:=0}, block_offset{dCol:=0, dRow:=1}, block_offset{dCol:=1, dRow:=1}, block_offset{dCol:=2, dRow:=1}}
            # 3: S Piece
            3 => array{block_offset{dCol:=1, dRow:=0}, block_offset{dCol:=2, dRow:=0}, block_offset{dCol:=0, dRow:=1}, block_offset{dCol:=1, dRow:=1}}
            # 4: Z Piece
            4 => array{block_offset{dCol:=0, dRow:=0}, block_offset{dCol:=1, dRow:=0}, block_offset{dCol:=1, dRow:=1}, block_offset{dCol:=2, dRow:=1}}
            # 5: J Piece
            5 => array{block_offset{dCol:=0, dRow:=0}, block_offset{dCol:=0, dRow:=1}, block_offset{dCol:=1, dRow:=1}, block_offset{dCol:=2, dRow:=1}}
            # 6: L Piece
            _ => array{block_offset{dCol:=2, dRow:=0}, block_offset{dCol:=0, dRow:=1}, block_offset{dCol:=1, dRow:=1}, block_offset{dCol:=2, dRow:=1}}
