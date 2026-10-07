#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>

constexpr int GRID_COLS = 10;
constexpr int GRID_ROWS = 20;

struct FIntPointOffset {
    int dCol;
    int dRow;
};

static const FIntPointOffset PIECE_SHAPES[8][4][4] = {
    // 0: None
    { {{0,0},{0,0},{0,0},{0,0}}, {{0,0},{0,0},{0,0},{0,0}}, {{0,0},{0,0},{0,0},{0,0}}, {{0,0},{0,0},{0,0},{0,0}} },
    // 1: I
    { {{-1,0},{0,0},{1,0},{2,0}}, {{1,-1},{1,0},{1,1},{1,2}}, {{-1,1},{0,1},{1,1},{2,1}}, {{0,-1},{0,0},{0,1},{0,2}} },
    // 2: O
    { {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}} },
    // 3: T
    { {{-1,0},{0,0},{1,0},{0,1}}, {{0,-1},{0,0},{0,1},{1,0}}, {{-1,0},{0,0},{1,0},{0,-1}}, {{0,-1},{0,0},{0,1},{-1,0}} },
    // 4: S
    { {{-1,0},{0,0},{0,1},{1,1}}, {{0,1},{0,0},{1,0},{1,-1}}, {{-1,-1},{0,-1},{0,0},{1,0}}, {{-1,1},{-1,0},{0,0},{0,-1}} },
    // 5: Z
    { {{-1,1},{0,1},{0,0},{1,0}}, {{1,1},{1,0},{0,0},{0,-1}}, {{-1,0},{0,0},{0,-1},{1,-1}}, {{0,1},{0,0},{-1,0},{-1,-1}} },
    // 6: J
    { {{-1,1},{-1,0},{0,0},{1,0}}, {{1,1},{0,1},{0,0},{0,-1}}, {{-1,0},{0,0},{1,0},{1,-1}}, {{0,1},{0,0},{0,-1},{-1,-1}} },
    // 7: L
    { {{-1,0},{0,0},{1,0},{1,1}}, {{0,1},{0,0},{0,-1},{1,-1}}, {{-1,-1},{-1,0},{0,0},{1,0}}, {{-1,1},{0,1},{0,0},{0,-1}} }
};

class TetrisSimulation {
public:
    int Grid[GRID_ROWS][GRID_COLS];
    int CurrentPieceType;
    int CurrentRotation;
    int CurrentCol;
    int CurrentRow;
    int NextPieceType;
    int Score;
    bool bGameOver;
    bool bSpawningEnabled;

    TetrisSimulation() {
        std::memset(Grid, 0, sizeof(Grid));
        CurrentPieceType = 1;
        CurrentRotation = 0;
        CurrentCol = 4;
        CurrentRow = 18;
        NextPieceType = 2;
        Score = 0;
        bGameOver = false;
        bSpawningEnabled = true;
    }

    bool IsValidPosition(int PieceType, int Rotation, int Col, int Row) const {
        for (int i = 0; i < 4; ++i) {
            int TargetCol = Col + PIECE_SHAPES[PieceType][Rotation][i].dCol;
            int TargetRow = Row + PIECE_SHAPES[PieceType][Rotation][i].dRow;

            if (TargetCol < 0 || TargetCol >= GRID_COLS || TargetRow < 0) {
                return false;
            }
            if (TargetRow < GRID_ROWS && Grid[TargetRow][TargetCol] != 0) {
                return false;
            }
        }
        return true;
    }

    void SpawnPiece(int PieceType) {
        CurrentPieceType = PieceType;
        CurrentRotation = 0;
        CurrentCol = 4;
        CurrentRow = 18;
    }

    void MoveLeft() {
        if (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol - 1, CurrentRow)) {
            CurrentCol--;
        }
    }

    void MoveRight() {
        if (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol + 1, CurrentRow)) {
            CurrentCol++;
        }
    }

    void RotatePiece() {
        int NewRot = (CurrentRotation + 1) % 4;
        if (IsValidPosition(CurrentPieceType, NewRot, CurrentCol, CurrentRow)) {
            CurrentRotation = NewRot;
        } else if (IsValidPosition(CurrentPieceType, NewRot, CurrentCol - 1, CurrentRow)) {
            CurrentCol--;
            CurrentRotation = NewRot;
        } else if (IsValidPosition(CurrentPieceType, NewRot, CurrentCol + 1, CurrentRow)) {
            CurrentCol++;
            CurrentRotation = NewRot;
        }
    }

    void HardDrop() {
        int DropDist = 0;
        while (IsValidPosition(CurrentPieceType, CurrentRotation, CurrentCol, CurrentRow - 1)) {
            CurrentRow--;
            DropDist++;
        }
        Score += DropDist * 2;
        LockPiece();
    }

    void LockPiece() {
        for (int i = 0; i < 4; ++i) {
            int C = CurrentCol + PIECE_SHAPES[CurrentPieceType][CurrentRotation][i].dCol;
            int R = CurrentRow + PIECE_SHAPES[CurrentPieceType][CurrentRotation][i].dRow;
            if (R >= 0 && R < GRID_ROWS && C >= 0 && C < GRID_COLS) {
                Grid[R][C] = CurrentPieceType;
            }
        }
        if (bSpawningEnabled) {
            SpawnPiece(NextPieceType);
        }
    }

    int GetOccupiedCount() const {
        int count = 0;
        for (int r = 0; r < GRID_ROWS; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                if (Grid[r][c] != 0) count++;
            }
        }
        return count;
    }

    int GetFreeCount() const {
        return (GRID_ROWS * GRID_COLS) - GetOccupiedCount();
    }

    void PrintGrid() const {
        std::cout << "--- Grid State (Top to Bottom) ---\n";
        for (int r = GRID_ROWS - 1; r >= 0; --r) {
            std::cout << (r < 10 ? " " : "") << r << " | ";
            for (int c = 0; c < GRID_COLS; ++c) {
                if (Grid[r][c] == 0) {
                    std::cout << ". ";
                } else {
                    std::cout << Grid[r][c] << " ";
                }
            }
            std::cout << "|\n";
        }
        std::cout << "   +--------------------+\n";
        std::cout << "     0 1 2 3 4 5 6 7 8 9 \n";
    }
};

int main() {
    std::cout << "========================================\n";
    std::cout << "TETRIS AUTOMATED SIMULATION TEST\n";
    std::cout << "========================================\n";

    TetrisSimulation sim;

    // Step 1: Start with an L piece (Piece 7)
    sim.SpawnPiece(7);
    std::cout << "[Step 1] Spawned L piece (Type 7). Start Col: " << sim.CurrentCol << ", Row: " << sim.CurrentRow << "\n";

    // Step 2: Rotate L piece
    sim.RotatePiece();
    std::cout << "[Step 2] Rotated piece. Rotation state: " << sim.CurrentRotation << "\n";

    // Step 3: Move left one slot
    sim.MoveLeft();
    std::cout << "[Step 3] Moved left 1 slot. CurrentCol: " << sim.CurrentCol << "\n";

    // Set next piece to S piece (Type 4)
    sim.NextPieceType = 4;

    // Step 4: Drop using space (HardDrop)
    std::cout << "[Step 4] Dropping piece using Space (HardDrop)...\n";
    sim.HardDrop();

    // Check occupied slots and free slots
    int occupied1 = sim.GetOccupiedCount();
    int free1 = sim.GetFreeCount();
    std::cout << ">> After Drop 1:\n";
    std::cout << "   Occupied Slots: " << occupied1 << " (Expected: 4)\n";
    std::cout << "   Free Slots:     " << free1 << " (Expected: 196)\n";
    assert(occupied1 == 4);
    assert(free1 == 196);
    std::cout << "   -> TEST 1 PASSED: Exactly 4 slots occupied!\n\n";

    // Step 5: The newly spawned piece is S piece (Type 4)
    std::cout << "[Step 5] S piece (Type 4) active at Col: " << sim.CurrentCol << ", Row: " << sim.CurrentRow << "\n";

    // Step 6: Move it one slot to the right
    sim.MoveRight();
    std::cout << "[Step 6] Moved right 1 slot. CurrentCol: " << sim.CurrentCol << "\n";

    // Step 7: Disable spawning of new pieces
    sim.bSpawningEnabled = false;
    std::cout << "[Step 7] Disabled spawning of new pieces.\n";

    // Step 8: Drop with Space
    std::cout << "[Step 8] Dropping piece using Space (HardDrop)...\n";
    sim.HardDrop();

    // Check occupied slots and free slots
    int occupied2 = sim.GetOccupiedCount();
    int free2 = sim.GetFreeCount();
    std::cout << ">> After Drop 2:\n";
    std::cout << "   Occupied Slots: " << occupied2 << " (Expected: 8)\n";
    std::cout << "   Free Slots:     " << free2 << " (Expected: 192)\n";
    assert(occupied2 == 8);
    assert(free2 == 192);
    std::cout << "   -> TEST 2 PASSED: Exactly 8 slots occupied!\n\n";

    sim.PrintGrid();

    std::cout << "========================================\n";
    std::cout << "ALL SIMULATION TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "========================================\n";
    return 0;
}
