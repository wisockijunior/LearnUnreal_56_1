using System.Collections.Generic;
using UnityEngine;

namespace LearnUnreal.Tetris
{
    /// <summary>
    /// Tetris Board Implementation in Unity3D C#.
    /// Manages 10x20 cell matrix, active tetromino piece, rotation states,
    /// wall kick checks, line clearing, score calculations, and cube block visuals.
    /// Direct equivalent to Unreal Engine's ATetrisBoardActor.
    /// </summary>
    public class TetrisBoard : MonoBehaviour
    {
        public const int GRID_COLS = 10;
        public const int GRID_ROWS = 20;
        public const float CELL_SIZE = 1.0f; // 1 Unity meter per grid cell

        // Tetromino shapes: 7 types, 4 rotations, 4 blocks each (col, row relative coordinates)
        private static readonly int[,,,] TETROMINO_SHAPES = new int[7, 4, 4, 2]
        {
            // 0: I (Cyan)
            {
                { {0,1}, {1,1}, {2,1}, {3,1} },
                { {2,0}, {2,1}, {2,2}, {2,3} },
                { {0,2}, {1,2}, {2,2}, {3,2} },
                { {1,0}, {1,1}, {1,2}, {1,3} }
            },
            // 1: O (Yellow)
            {
                { {1,0}, {2,0}, {1,1}, {2,1} },
                { {1,0}, {2,0}, {1,1}, {2,1} },
                { {1,0}, {2,0}, {1,1}, {2,1} },
                { {1,0}, {2,0}, {1,1}, {2,1} }
            },
            // 2: T (Purple)
            {
                { {1,0}, {0,1}, {1,1}, {2,1} },
                { {1,0}, {1,1}, {2,1}, {1,2} },
                { {0,1}, {1,1}, {2,1}, {1,2} },
                { {1,0}, {0,1}, {1,1}, {1,2} }
            },
            // 3: S (Green)
            {
                { {1,0}, {2,0}, {0,1}, {1,1} },
                { {1,0}, {1,1}, {2,1}, {2,2} },
                { {1,1}, {2,1}, {0,2}, {1,2} },
                { {0,0}, {0,1}, {1,1}, {1,2} }
            },
            // 4: Z (Red)
            {
                { {0,0}, {1,0}, {1,1}, {2,1} },
                { {2,0}, {1,1}, {2,1}, {1,2} },
                { {0,1}, {1,1}, {1,2}, {2,2} },
                { {1,0}, {0,1}, {1,1}, {0,2} }
            },
            // 5: J (Blue)
            {
                { {0,0}, {0,1}, {1,1}, {2,1} },
                { {1,0}, {2,0}, {1,1}, {1,2} },
                { {0,1}, {1,1}, {2,1}, {2,2} },
                { {1,0}, {1,1}, {0,2}, {1,2} }
            },
            // 6: L (Orange)
            {
                { {2,0}, {0,1}, {1,1}, {2,1} },
                { {1,0}, {1,1}, {1,2}, {2,2} },
                { {0,1}, {1,1}, {2,1}, {0,2} },
                { {0,0}, {1,0}, {1,1}, {1,2} }
            }
        };

        private static readonly Color[] PIECE_COLORS = new Color[]
        {
            new Color(0.0f, 0.85f, 0.95f), // Cyan (I)
            new Color(0.95f, 0.85f, 0.05f), // Yellow (O)
            new Color(0.65f, 0.15f, 0.85f), // Purple (T)
            new Color(0.15f, 0.85f, 0.25f), // Green (S)
            new Color(0.90f, 0.15f, 0.15f), // Red (Z)
            new Color(0.15f, 0.35f, 0.90f), // Blue (J)
            new Color(0.95f, 0.55f, 0.10f)  // Orange (L)
        };

        // Grid state: 0 = empty, 1..7 = piece color index + 1
        private int[,] grid = new int[GRID_ROWS, GRID_COLS];

        // Active falling piece
        private int currentPieceType;
        private int currentRotation;
        private int currentCol;
        private int currentRow;

        // Next preview piece
        private int nextPieceType;

        // Gameplay progression
        private float baseDropInterval = 0.8f;
        private float currentDropInterval;
        private float dropTimer;
        private int score;
        private int linesCleared;
        private int level = 1;
        private bool isGameOver;

        // Visual blocks tracking
        private GameObject[,] lockedBlockObjects = new GameObject[GRID_ROWS, GRID_COLS];
        private GameObject[] activePieceObjects = new GameObject[4];
        private GameObject[] nextPieceObjects = new GameObject[4];
        private Transform boardVisualRoot;

        public int Score => score;
        public int LinesCleared => linesCleared;
        public int Level => level;
        public int NextPieceType => nextPieceType;
        public bool IsGameOver => isGameOver;

        private void Awake()
        {
            boardVisualRoot = new GameObject("BoardVisualRoot").transform;
            boardVisualRoot.SetParent(transform);
            CreateGridFrame();
            InitActiveVisuals();
            RestartGame();
        }

        private void Update()
        {
            if (isGameOver)
            {
                if (Input.GetKeyDown(KeyCode.R))
                {
                    RestartGame();
                }
                return;
            }

            HandleInput();

            // Automatic gravity tick
            dropTimer += Time.deltaTime;
            if (dropTimer >= currentDropInterval)
            {
                dropTimer = 0.0f;
                StepDrop();
            }
        }

        private void HandleInput()
        {
            if (Input.GetKeyDown(KeyCode.A) || Input.GetKeyDown(KeyCode.LeftArrow))
            {
                MoveLeft();
            }
            else if (Input.GetKeyDown(KeyCode.D) || Input.GetKeyDown(KeyCode.RightArrow))
            {
                MoveRight();
            }
            else if (Input.GetKeyDown(KeyCode.W) || Input.GetKeyDown(KeyCode.UpArrow))
            {
                RotatePiece();
            }
            else if (Input.GetKeyDown(KeyCode.S) || Input.GetKeyDown(KeyCode.DownArrow))
            {
                SoftDrop();
            }
            else if (Input.GetKeyDown(KeyCode.Space) || Input.GetKeyDown(KeyCode.Return))
            {
                HardDrop();
            }
        }

        public void MoveLeft()
        {
            if (IsValidPosition(currentPieceType, currentRotation, currentCol - 1, currentRow))
            {
                currentCol--;
                UpdateActivePieceVisuals();
            }
        }

        public void MoveRight()
        {
            if (IsValidPosition(currentPieceType, currentRotation, currentCol + 1, currentRow))
            {
                currentCol++;
                UpdateActivePieceVisuals();
            }
        }

        public void RotatePiece()
        {
            int nextRot = (currentRotation + 1) % 4;

            // Basic rotation & wall-kick checks
            if (IsValidPosition(currentPieceType, nextRot, currentCol, currentRow))
            {
                currentRotation = nextRot;
            }
            else if (IsValidPosition(currentPieceType, nextRot, currentCol - 1, currentRow))
            {
                currentCol--;
                currentRotation = nextRot;
            }
            else if (IsValidPosition(currentPieceType, nextRot, currentCol + 1, currentRow))
            {
                currentCol++;
                currentRotation = nextRot;
            }
            else if (IsValidPosition(currentPieceType, nextRot, currentCol, currentRow + 1))
            {
                currentRow++;
                currentRotation = nextRot;
            }

            UpdateActivePieceVisuals();
        }

        public void SoftDrop()
        {
            if (IsValidPosition(currentPieceType, currentRotation, currentCol, currentRow - 1))
            {
                currentRow--;
                score += 1;
                dropTimer = 0.0f;
                UpdateActivePieceVisuals();
            }
            else
            {
                LockPiece();
            }
        }

        public void HardDrop()
        {
            int dropDist = 0;
            while (IsValidPosition(currentPieceType, currentRotation, currentCol, currentRow - 1))
            {
                currentRow--;
                dropDist++;
            }
            score += dropDist * 2;
            LockPiece();
        }

        private void StepDrop()
        {
            if (IsValidPosition(currentPieceType, currentRotation, currentCol, currentRow - 1))
            {
                currentRow--;
                UpdateActivePieceVisuals();
            }
            else
            {
                LockPiece();
            }
        }

        private void LockPiece()
        {
            for (int i = 0; i < 4; i++)
            {
                int c = currentCol + TETROMINO_SHAPES[currentPieceType, currentRotation, i, 0];
                int r = currentRow - TETROMINO_SHAPES[currentPieceType, currentRotation, i, 1];

                if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS)
                {
                    grid[r, c] = currentPieceType + 1;
                }
            }

            ClearLines();
            SpawnNewPiece();
        }

        private void ClearLines()
        {
            int clearedThisTurn = 0;

            for (int r = 0; r < GRID_ROWS; r++)
            {
                bool full = true;
                for (int c = 0; c < GRID_COLS; c++)
                {
                    if (grid[r, c] == 0)
                    {
                        full = false;
                        break;
                    }
                }

                if (full)
                {
                    clearedThisTurn++;
                    // Shift lines down
                    for (int y = r; y < GRID_ROWS - 1; y++)
                    {
                        for (int c = 0; c < GRID_COLS; c++)
                        {
                            grid[y, c] = grid[y + 1, c];
                        }
                    }
                    for (int c = 0; c < GRID_COLS; c++)
                    {
                        grid[GRID_ROWS - 1, c] = 0;
                    }
                    r--; // Recheck current row index
                }
            }

            if (clearedThisTurn > 0)
            {
                linesCleared += clearedThisTurn;

                // Classic Tetris scoring
                int[] pointsTable = { 0, 100, 300, 500, 800 };
                score += pointsTable[Mathf.Min(clearedThisTurn, 4)] * level;

                // Level progression every 10 lines
                level = 1 + (linesCleared / 10);
                currentDropInterval = Mathf.Max(0.08f, baseDropInterval * Mathf.Pow(0.85f, level - 1));
            }

            RebuildLockedVisuals();
        }

        private void SpawnNewPiece()
        {
            currentPieceType = nextPieceType;
            nextPieceType = Random.Range(0, 7);
            currentRotation = 0;
            currentCol = 3;
            currentRow = GRID_ROWS - 1;

            if (!IsValidPosition(currentPieceType, currentRotation, currentCol, currentRow))
            {
                isGameOver = true;
                return;
            }

            UpdateActivePieceVisuals();
            UpdateNextPieceVisuals();
        }

        public bool IsValidPosition(int pieceType, int rot, int col, int row)
        {
            for (int i = 0; i < 4; i++)
            {
                int c = col + TETROMINO_SHAPES[pieceType, rot, i, 0];
                int r = row - TETROMINO_SHAPES[pieceType, rot, i, 1];

                if (c < 0 || c >= GRID_COLS || r < 0)
                {
                    return false;
                }

                if (r < GRID_ROWS && grid[r, c] != 0)
                {
                    return false;
                }
            }
            return true;
        }

        public void RestartGame()
        {
            for (int r = 0; r < GRID_ROWS; r++)
            {
                for (int c = 0; c < GRID_COLS; c++)
                {
                    grid[r, c] = 0;
                }
            }

            score = 0;
            linesCleared = 0;
            level = 1;
            isGameOver = false;
            currentDropInterval = baseDropInterval;
            dropTimer = 0.0f;

            nextPieceType = Random.Range(0, 7);
            SpawnNewPiece();
            RebuildLockedVisuals();
        }

        // =========================================================================
        // VISUALS & PIVOT ALIGNMENT
        // =========================================================================

        private void InitActiveVisuals()
        {
            for (int i = 0; i < 4; i++)
            {
                GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
                cube.name = $"ActiveBlock_{i}";
                cube.transform.SetParent(boardVisualRoot);
                cube.transform.localScale = Vector3.one * (CELL_SIZE * 0.95f);
                activePieceObjects[i] = cube;

                GameObject nextCube = GameObject.CreatePrimitive(PrimitiveType.Cube);
                nextCube.name = $"NextBlock_{i}";
                nextCube.transform.SetParent(boardVisualRoot);
                nextCube.transform.localScale = Vector3.one * (CELL_SIZE * 0.95f);
                nextPieceObjects[i] = nextCube;
            }
        }

        /// <summary>
        /// Translates 2D grid coordinates (col, row) into Unity World coordinates.
        /// Accounts for Unity center pivots: primitive cube center aligns with (col * size, row * size).
        /// </summary>
        public Vector3 GridToWorld(int col, int row)
        {
            float originX = - (GRID_COLS * CELL_SIZE) * 0.5f + (CELL_SIZE * 0.5f);
            float originY = - (GRID_ROWS * CELL_SIZE) * 0.5f + (CELL_SIZE * 0.5f);
            return transform.position + new Vector3(originX + col * CELL_SIZE, originY + row * CELL_SIZE, 0.0f);
        }

        private void UpdateActivePieceVisuals()
        {
            Color pieceCol = PIECE_COLORS[currentPieceType];
            for (int i = 0; i < 4; i++)
            {
                int c = currentCol + TETROMINO_SHAPES[currentPieceType, currentRotation, i, 0];
                int r = currentRow - TETROMINO_SHAPES[currentPieceType, currentRotation, i, 1];

                if (r < GRID_ROWS)
                {
                    activePieceObjects[i].SetActive(true);
                    activePieceObjects[i].transform.position = GridToWorld(c, r);
                    activePieceObjects[i].GetComponent<Renderer>().material.color = pieceCol;
                }
                else
                {
                    activePieceObjects[i].SetActive(false);
                }
            }
        }

        private void UpdateNextPieceVisuals()
        {
            Color nextCol = PIECE_COLORS[nextPieceType];
            Vector3 previewOrigin = transform.position + new Vector3((GRID_COLS * CELL_SIZE * 0.5f) + 3.0f, 4.0f, 0.0f);

            for (int i = 0; i < 4; i++)
            {
                int c = TETROMINO_SHAPES[nextPieceType, 0, i, 0];
                int r = -TETROMINO_SHAPES[nextPieceType, 0, i, 1];

                nextPieceObjects[i].SetActive(true);
                nextPieceObjects[i].transform.position = previewOrigin + new Vector3(c * CELL_SIZE, r * CELL_SIZE, 0.0f);
                nextPieceObjects[i].GetComponent<Renderer>().material.color = nextCol;
            }
        }

        private void RebuildLockedVisuals()
        {
            for (int r = 0; r < GRID_ROWS; r++)
            {
                for (int c = 0; c < GRID_COLS; c++)
                {
                    int val = grid[r, c];
                    if (val > 0)
                    {
                        if (lockedBlockObjects[r, c] == null)
                        {
                            GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
                            cube.name = $"Locked_{r}_{c}";
                            cube.transform.SetParent(boardVisualRoot);
                            cube.transform.localScale = Vector3.one * (CELL_SIZE * 0.95f);
                            lockedBlockObjects[r, c] = cube;
                        }
                        lockedBlockObjects[r, c].SetActive(true);
                        lockedBlockObjects[r, c].transform.position = GridToWorld(c, r);
                        lockedBlockObjects[r, c].GetComponent<Renderer>().material.color = PIECE_COLORS[val - 1];
                    }
                    else
                    {
                        if (lockedBlockObjects[r, c] != null)
                        {
                            lockedBlockObjects[r, c].SetActive(false);
                        }
                    }
                }
            }
        }

        private void CreateGridFrame()
        {
            // Frame border around the 10x20 playfield
            float frameThickness = 0.3f;
            float totalW = GRID_COLS * CELL_SIZE;
            float totalH = GRID_ROWS * CELL_SIZE;

            // Left wall
            CreateFrameBar(new Vector3(-totalW * 0.5f - frameThickness * 0.5f, 0.0f, 0.0f),
                           new Vector3(frameThickness, totalH + frameThickness * 2.0f, 0.5f));

            // Right wall
            CreateFrameBar(new Vector3(totalW * 0.5f + frameThickness * 0.5f, 0.0f, 0.0f),
                           new Vector3(frameThickness, totalH + frameThickness * 2.0f, 0.5f));

            // Floor
            CreateFrameBar(new Vector3(0.0f, -totalH * 0.5f - frameThickness * 0.5f, 0.0f),
                           new Vector3(totalW + frameThickness * 2.0f, frameThickness, 0.5f));
        }

        private void CreateFrameBar(Vector3 localPos, Vector3 localScale)
        {
            GameObject bar = GameObject.CreatePrimitive(PrimitiveType.Cube);
            bar.name = "FrameBorder";
            bar.transform.SetParent(boardVisualRoot);
            bar.transform.localPosition = localPos;
            bar.transform.localScale = localScale;
            bar.GetComponent<Renderer>().material.color = new Color(0.2f, 0.25f, 0.35f);
        }
    }
}
