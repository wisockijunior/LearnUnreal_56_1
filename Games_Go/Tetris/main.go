// Tetris in pure Go (Golang)
// Zero-dependency terminal game using Windows console and standard library.
// Demonstrates Go slices, arrays, concurrency (tickers), and 10x20 matrix mechanics.

package main

import (
	"fmt"
	"math"
	"strings"
	"sync"
	"syscall"
	"time"
)

const (
	GridCols = 10
	GridRows = 20
)

type BlockOffset struct {
	dCol int
	dRow int
}

var TetrominoShapes = [7][4][4]BlockOffset{
	// 0: I (Cyan)
	{
		{{0, 1}, {1, 1}, {2, 1}, {3, 1}},
		{{2, 0}, {2, 1}, {2, 2}, {2, 3}},
		{{0, 2}, {1, 2}, {2, 2}, {3, 2}},
		{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
	},
	// 1: O (Yellow)
	{
		{{1, 0}, {2, 0}, {1, 1}, {2, 1}},
		{{1, 0}, {2, 0}, {1, 1}, {2, 1}},
		{{1, 0}, {2, 0}, {1, 1}, {2, 1}},
		{{1, 0}, {2, 0}, {1, 1}, {2, 1}},
	},
	// 2: T (Purple)
	{
		{{1, 0}, {0, 1}, {1, 1}, {2, 1}},
		{{1, 0}, {1, 1}, {2, 1}, {1, 2}},
		{{0, 1}, {1, 1}, {2, 1}, {1, 2}},
		{{1, 0}, {0, 1}, {1, 1}, {1, 2}},
	},
	// 3: S (Green)
	{
		{{1, 0}, {2, 0}, {0, 1}, {1, 1}},
		{{1, 0}, {1, 1}, {2, 1}, {2, 2}},
		{{1, 1}, {2, 1}, {0, 2}, {1, 2}},
		{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
	},
	// 4: Z (Red)
	{
		{{0, 0}, {1, 0}, {1, 1}, {2, 1}},
		{{2, 0}, {1, 1}, {2, 1}, {1, 2}},
		{{0, 1}, {1, 1}, {1, 2}, {2, 2}},
		{{1, 0}, {0, 1}, {1, 1}, {0, 2}},
	},
	// 5: J (Blue)
	{
		{{0, 0}, {0, 1}, {1, 1}, {2, 1}},
		{{1, 0}, {2, 0}, {1, 1}, {1, 2}},
		{{0, 1}, {1, 1}, {2, 1}, {2, 2}},
		{{1, 0}, {1, 1}, {0, 2}, {1, 2}},
	},
	// 6: L (Orange)
	{
		{{2, 0}, {0, 1}, {1, 1}, {2, 1}},
		{{1, 0}, {1, 1}, {1, 2}, {2, 2}},
		{{0, 1}, {1, 1}, {2, 1}, {0, 2}},
		{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
	},
}

// ANSI background color codes for the 7 pieces
var PieceAnsiColors = []string{
	"\033[46;30m", // Cyan (I)
	"\033[43;30m", // Yellow (O)
	"\033[45;37m", // Magenta (T)
	"\033[42;30m", // Green (S)
	"\033[41;37m", // Red (Z)
	"\033[44;37m", // Blue (J)
	"\033[47;30m", // White / Orange (L)
}

type TetrisGame struct {
	mu           sync.Mutex
	grid         [GridRows][GridCols]int
	currentPiece int
	currentRot   int
	currentCol   int
	currentRow   int
	nextPiece    int
	score        int
	linesCleared int
	level        int
	isGameOver   bool
	dropInterval time.Duration
	lastDrop     time.Time
}

var (
	msvcrt      = syscall.NewLazyDLL("msvcrt.dll")
	procKbhit   = msvcrt.NewProc("_kbhit")
	procGetch   = msvcrt.NewProc("_getch")
)

func kbhit() bool {
	r, _, _ := procKbhit.Call()
	return r != 0
}

func getch() int {
	r, _, _ := procGetch.Call()
	return int(r)
}

func NewTetrisGame() *TetrisGame {
	g := &TetrisGame{
		dropInterval: 800 * time.Millisecond,
		lastDrop:     time.Now(),
		nextPiece:    1,
	}
	g.Restart()
	return g
}

func (g *TetrisGame) Restart() {
	g.grid = [GridRows][GridCols]int{}
	g.score = 0
	g.linesCleared = 0
	g.level = 1
	g.isGameOver = false
	g.dropInterval = 800 * time.Millisecond
	g.lastDrop = time.Now()
	g.currentPiece = 0
	g.nextPiece = 1
	g.spawnNewPiece()
}

func (g *TetrisGame) spawnNewPiece() {
	g.currentPiece = g.nextPiece
	nanos := int(time.Now().UnixNano() % 7)
	g.nextPiece = (g.score + g.linesCleared + nanos) % 7
	g.currentRot = 0
	g.currentCol = 3
	g.currentRow = 0

	if !g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol, g.currentRow) {
		g.isGameOver = true
	}
}

func (g *TetrisGame) isValidPosition(piece, rot, col, row int) bool {
	blocks := TetrominoShapes[piece][rot]
	for _, b := range blocks {
		c := col + b.dCol
		r := row + b.dRow

		if c < 0 || c >= GridCols || r >= GridRows {
			return false
		}
		if r >= 0 && g.grid[r][c] != 0 {
			return false
		}
	}
	return true
}

func (g *TetrisGame) MoveLeft() {
	if g.isGameOver { return }
	if g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol-1, g.currentRow) {
		g.currentCol--
	}
}

func (g *TetrisGame) MoveRight() {
	if g.isGameOver { return }
	if g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol+1, g.currentRow) {
		g.currentCol++
	}
}

func (g *TetrisGame) Rotate() {
	if g.isGameOver { return }
	nextRot := (g.currentRot + 1) % 4

	if g.isValidPosition(g.currentPiece, nextRot, g.currentCol, g.currentRow) {
		g.currentRot = nextRot
	} else if g.isValidPosition(g.currentPiece, nextRot, g.currentCol-1, g.currentRow) {
		g.currentCol--
		g.currentRot = nextRot
	} else if g.isValidPosition(g.currentPiece, nextRot, g.currentCol+1, g.currentRow) {
		g.currentCol++
		g.currentRot = nextRot
	}
}

func (g *TetrisGame) SoftDrop() {
	if g.isGameOver { return }
	if g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol, g.currentRow+1) {
		g.currentRow++
		g.score++
		g.lastDrop = time.Now()
	} else {
		g.lockPiece()
	}
}

func (g *TetrisGame) HardDrop() {
	if g.isGameOver { return }
	dist := 0
	for g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol, g.currentRow+1) {
		g.currentRow++
		dist++
	}
	g.score += dist * 2
	g.lockPiece()
}

func (g *TetrisGame) stepDrop() {
	if g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol, g.currentRow+1) {
		g.currentRow++
	} else {
		g.lockPiece()
	}
}

func (g *TetrisGame) lockPiece() {
	blocks := TetrominoShapes[g.currentPiece][g.currentRot]
	for _, b := range blocks {
		c := g.currentCol + b.dCol
		r := g.currentRow + b.dRow
		if r >= 0 && r < GridRows && c >= 0 && c < GridCols {
			g.grid[r][c] = g.currentPiece + 1
		}
	}
	g.clearLines()
	g.spawnNewPiece()
}

func (g *TetrisGame) clearLines() {
	cleared := 0
	r := GridRows - 1

	for r >= 0 {
		isFull := true
		for c := 0; c < GridCols; c++ {
			if g.grid[r][c] == 0 {
				isFull = false
				break
			}
		}

		if isFull {
			cleared++
			for y := r; y > 0; y-- {
				g.grid[y] = g.grid[y-1]
			}
			g.grid[0] = [GridCols]int{}
		} else {
			r--
		}
	}

	if cleared > 0 {
		g.linesCleared += cleared
		points := []int{0, 100, 300, 500, 800}
		idx := int(math.Min(float64(cleared), 4))
		g.score += points[idx] * g.level
		g.level = 1 + (g.linesCleared / 10)

		speedMs := math.Max(80.0, 800.0*math.Pow(0.85, float64(g.level-1)))
		g.dropInterval = time.Duration(speedMs) * time.Millisecond
	}
}

func (g *TetrisGame) getGhostRow() int {
	ghost := g.currentRow
	for g.isValidPosition(g.currentPiece, g.currentRot, g.currentCol, ghost+1) {
		ghost++
	}
	return ghost
}

func (g *TetrisGame) Update() {
	g.mu.Lock()
	defer g.mu.Unlock()

	if !g.isGameOver && time.Since(g.lastDrop) >= g.dropInterval {
		g.lastDrop = time.Now()
		g.stepDrop()
	}
}

func (g *TetrisGame) Render() {
	g.mu.Lock()
	defer g.mu.Unlock()

	var sb strings.Builder
	sb.WriteString("\033[H") // Top-left cursor

	ghostR := g.getGhostRow()
	activeBlocks := TetrominoShapes[g.currentPiece][g.currentRot]

	sb.WriteString("  \033[90m+--------------------+\033[0m   \033[1;33mTETRIS (GOLANG)\033[0m\n")

	for r := 0; r < GridRows; r++ {
		sb.WriteString("  \033[90m|\033[0m")

		for c := 0; c < GridCols; c++ {
			cell := g.grid[r][c]

			isActive := false
			if !g.isGameOver {
				for _, b := range activeBlocks {
					if g.currentCol+b.dCol == c && g.currentRow+b.dRow == r {
						isActive = true
						break
					}
				}
			}

			isGhost := false
			if !g.isGameOver && ghostR != g.currentRow {
				for _, b := range activeBlocks {
					if g.currentCol+b.dCol == c && ghostR+b.dRow == r {
						isGhost = true
						break
					}
				}
			}

			if cell > 0 {
				sb.WriteString(PieceAnsiColors[cell-1] + "  \033[0m")
			} else if isActive {
				sb.WriteString(PieceAnsiColors[g.currentPiece] + "[]\033[0m")
			} else if isGhost {
				sb.WriteString("\033[36m::\033[0m")
			} else {
				sb.WriteString("\033[90m. \033[0m")
			}
		}

		sb.WriteString("\033[90m|\033[0m")

		// Sidebar lines
		switch r {
		case 1:
			sb.WriteString(fmt.Sprintf("   SCORE: \033[1;37m%d\033[0m", g.score))
		case 3:
			sb.WriteString(fmt.Sprintf("   LINES: \033[1;37m%d\033[0m", g.linesCleared))
		case 5:
			sb.WriteString(fmt.Sprintf("   LEVEL: \033[1;33m%d\033[0m", g.level))
		case 8:
			sb.WriteString("   NEXT PIECE:")
		case 9:
			sb.WriteString("   \033[90m+--------+\033[0m")
		case 10, 11, 12, 13:
			pr := r - 10
			sb.WriteString("   \033[90m|\033[0m")
			for pc := 0; pc < 4; pc++ {
				hasBlock := false
				for _, b := range TetrominoShapes[g.nextPiece][0] {
					if b.dCol == pc && b.dRow == pr {
						hasBlock = true
						break
					}
				}
				if hasBlock {
					sb.WriteString(PieceAnsiColors[g.nextPiece] + "  \033[0m")
				} else {
					sb.WriteString("  ")
				}
			}
			sb.WriteString("\033[90m|\033[0m")
		case 14:
			sb.WriteString("   \033[90m+--------+\033[0m")
		case 16:
			sb.WriteString("   A / D : Move")
		case 17:
			sb.WriteString("   W     : Rotate")
		case 18:
			sb.WriteString("   S     : Soft Drop")
		case 19:
			sb.WriteString("   SPACE : Hard Drop | R: Restart | Q: Quit")
		}

		sb.WriteString("\n")
	}

	sb.WriteString("  \033[90m+--------------------+\033[0m\n")

	if g.isGameOver {
		sb.WriteString("       \033[1;41;37m GAME OVER! Press R to Restart \033[0m\n")
	}

	fmt.Print(sb.String())
}

func main() {
	fmt.Print("\033[?25l\033[2J") // Hide cursor, clear screen
	defer fmt.Print("\033[?25h\033[0m\n")

	game := NewTetrisGame()
	ticker := time.NewTicker(16 * time.Millisecond) // ~60 FPS
	defer ticker.Stop()

	running := true
	for running {
		for kbhit() {
			ch := getch()
			if ch == 0 || ch == 224 {
				arrow := getch()
				switch arrow {
				case 75: // Left
					game.MoveLeft()
				case 77: // Right
					game.MoveRight()
				case 72: // Up
					game.Rotate()
				case 80: // Down
					game.SoftDrop()
				}
			} else if ch == 'q' || ch == 'Q' {
				running = false
				break
			} else if ch == 'a' || ch == 'A' {
				game.MoveLeft()
			} else if ch == 'd' || ch == 'D' {
				game.MoveRight()
			} else if ch == 'w' || ch == 'W' {
				game.Rotate()
			} else if ch == 's' || ch == 'S' {
				game.SoftDrop()
			} else if ch == ' ' || ch == 13 {
				game.HardDrop()
			} else if ch == 'r' || ch == 'R' {
				game.Restart()
			}
		}

		game.Update()
		game.Render()

		<-ticker.C
	}

	fmt.Println("Thanks for playing Tetris in Go!")
}
