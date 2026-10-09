// Flappy Bird in pure Go (Golang)
// Zero-dependency terminal game using Windows console and standard library.
// Demonstrates Go concurrency (goroutines, channels, tickers), structs, and game loop design.

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
	ScreenWidth    = 50
	ScreenHeight   = 24
	PlayableHeight = 20

	Gravity      = 0.52
	FlapStrength = -2.8
	MaxFallSpeed = 4.0

	PipeWidth    = 5
	PipeGapSize  = 6
	PipeSpeed    = 0.8
	SpawnInterval = 2.0
)

type GameState int

const (
	StateReady GameState = iota
	StatePlaying
	StateGameOver
)

type PipePair struct {
	X      float64
	GapY   int
	Scored bool
}

type FlappyGame struct {
	mu        sync.Mutex
	state     GameState
	birdY     float64
	birdVy    float64
	score     int
	highScore int
	pipes     []PipePair
	lastSpawn time.Time
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

func NewFlappyGame() *FlappyGame {
	return &FlappyGame{
		state:     StateReady,
		birdY:     float64(PlayableHeight / 2),
		birdVy:    0,
		score:     0,
		highScore: 0,
		pipes:     make([]PipePair, 0),
		lastSpawn: time.Now(),
	}
}

func (g *FlappyGame) Reset() {
	g.state = StateReady
	g.birdY = float64(PlayableHeight / 2)
	g.birdVy = 0
	g.score = 0
	g.pipes = make([]PipePair, 0)
	g.lastSpawn = time.Now()
}

func (g *FlappyGame) Flap() {
	switch g.state {
	case StateReady:
		g.state = StatePlaying
		g.birdVy = FlapStrength
	case StatePlaying:
		g.birdVy = FlapStrength
	case StateGameOver:
		g.Reset()
	}
}

func (g *FlappyGame) Die() {
	g.state = StateGameOver
	if g.score > g.highScore {
		g.highScore = g.score
	}
}

func (g *FlappyGame) Update() {
	g.mu.Lock()
	defer g.mu.Unlock()

	switch g.state {
	case StateReady:
		g.birdY = float64(PlayableHeight / 2)

	case StatePlaying:
		// Physics
		g.birdVy += Gravity
		if g.birdVy > MaxFallSpeed {
			g.birdVy = MaxFallSpeed
		}
		g.birdY += g.birdVy

		if g.birdY < 1.0 {
			g.birdY = 1.0
			g.birdVy = 0.0
		}
		if g.birdY >= float64(PlayableHeight-1) {
			g.birdY = float64(PlayableHeight - 1)
			g.Die()
			return
		}

		// Pipe spawn
		if time.Since(g.lastSpawn).Seconds() >= SpawnInterval {
			g.lastSpawn = time.Now()
			seed := (g.score*7 + int(time.Now().UnixNano()%9)) % 7
			gapY := 4 + seed
			g.pipes = append(g.pipes, PipePair{
				X:      float64(ScreenWidth),
				GapY:   gapY,
				Scored: false,
			})
		}

		// Update pipes & AABB collision
		birdX := 10
		birdYInt := int(math.Round(g.birdY))
		newPipes := make([]PipePair, 0, len(g.pipes))

		for i := range g.pipes {
			p := &g.pipes[i]
			p.X -= PipeSpeed

			px := int(math.Round(p.X))
			halfGap := PipeGapSize / 2
			topBottom := p.GapY - halfGap
			botTop := p.GapY + halfGap

			// Score gate
			if !p.Scored && px+PipeWidth < birdX {
				p.Scored = true
				g.score++
			}

			// Collision check
			if birdX >= px && birdX < px+PipeWidth {
				if birdYInt <= topBottom || birdYInt >= botTop {
					g.Die()
					return
				}
			}

			if p.X > float64(-PipeWidth) {
				newPipes = append(newPipes, *p)
			}
		}
		g.pipes = newPipes

	case StateGameOver:
		if g.birdY < float64(PlayableHeight-1) {
			g.birdVy += Gravity
			g.birdY += g.birdVy
		} else {
			g.birdY = float64(PlayableHeight - 1)
		}
	}
}

func (g *FlappyGame) Render() {
	g.mu.Lock()
	defer g.mu.Unlock()

	var sb strings.Builder
	// Move cursor to top-left
	sb.WriteString("\033[H")

	// 1. Draw Sky & Pipes
	birdX := 10
	birdYInt := int(math.Round(g.birdY))

	for y := 0; y < PlayableHeight; y++ {
		for x := 0; x < ScreenWidth; x++ {
			// Check bird
			if x == birdX && y == birdYInt {
				sb.WriteString("\033[43;30m@>\033[0m")
				x++
				continue
			}

			// Check pipe
			isPipe := false
			for _, p := range g.pipes {
				px := int(math.Round(p.X))
				halfGap := PipeGapSize / 2
				topBottom := p.GapY - halfGap
				botTop := p.GapY + halfGap

				if x >= px && x < px+PipeWidth {
					if y <= topBottom || y >= botTop {
						isPipe = true
						break
					}
				}
			}

			if isPipe {
				sb.WriteString("\033[42;30m#\033[0m")
			} else {
				sb.WriteString("\033[46m \033[0m") // Sky blue
			}
		}
		sb.WriteString("\n")
	}

	// 2. Draw Ground
	sb.WriteString("\033[42;37m" + strings.Repeat("=", ScreenWidth) + "\033[0m\n")
	for y := PlayableHeight + 1; y < ScreenHeight; y++ {
		sb.WriteString("\033[43m" + strings.Repeat(" ", ScreenWidth) + "\033[0m\n")
	}

	// 3. Overlays / HUD
	switch g.state {
	case StateReady:
		sb.WriteString(fmt.Sprintf("\033[1;33m  FLAPPY BIRD (GOLANG)  \033[0m | Press SPACE/UP to Flap | High: %d\n", g.highScore))
	case StatePlaying:
		sb.WriteString(fmt.Sprintf("\033[1;37m  SCORE: %-4d\033[0m  High Score: %-4d | Press Q to Quit\n", g.score, g.highScore))
	case StateGameOver:
		sb.WriteString(fmt.Sprintf("\033[1;31m  GAME OVER! Score: %d | High: %d\033[0m | Press SPACE/R to Restart\n", g.score, g.highScore))
	}

	fmt.Print(sb.String())
}

func main() {
	// Hide cursor and clear screen
	fmt.Print("\033[?25l\033[2J")
	defer fmt.Print("\033[?25h\033[0m\n")

	game := NewFlappyGame()
	ticker := time.NewTicker(33 * time.Millisecond) // ~30 FPS
	defer ticker.Stop()

	running := true
	for running {
		// Non-blocking keyboard input on Windows
		for kbhit() {
			ch := getch()
			if ch == 0 || ch == 224 {
				// Special key prefix
				arrow := getch()
				if arrow == 72 { // Up arrow
					game.Flap()
				}
			} else if ch == 'q' || ch == 'Q' {
				running = false
				break
			} else if ch == ' ' || ch == 'w' || ch == 'W' {
				game.Flap()
			} else if ch == 'r' || ch == 'R' {
				if game.state == StateGameOver {
					game.Reset()
				}
			}
		}

		game.Update()
		game.Render()

		<-ticker.C
	}

	fmt.Println("Thanks for playing Flappy Bird in Go!")
}
