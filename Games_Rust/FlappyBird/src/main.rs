//! Flappy Bird in Rust (crossterm)
//! High-performance, memory-safe, zero-GC implementation for learning and engine comparison.

use crossterm::{
    cursor::{Hide, MoveTo, Show},
    event::{self, Event, KeyCode, KeyEventKind},
    execute,
    style::{Color, Print, ResetColor, SetBackgroundColor, SetForegroundColor},
    terminal::{disable_raw_mode, enable_raw_mode, Clear, ClearType, EnterAlternateScreen, LeaveAlternateScreen},
};
use std::io::{stdout, Result, Stdout};
use std::time::{Duration, Instant};

const SCREEN_WIDTH: u16 = 50;
const SCREEN_HEIGHT: u16 = 24;
const PLAYABLE_HEIGHT: u16 = 20;

const GRAVITY: f32 = 0.52;
const FLAP_STRENGTH: f32 = -2.8;
const MAX_FALL_SPEED: f32 = 4.0;

const PIPE_WIDTH: i16 = 5;
const PIPE_GAP_SIZE: i16 = 6;
const PIPE_SPEED: f32 = 0.8;
const SPAWN_INTERVAL_SECS: f32 = 2.0;

#[derive(Clone, Copy, PartialEq, Eq)]
enum GameState {
    Ready,
    Playing,
    GameOver,
}

#[derive(Clone, Copy)]
struct PipePair {
    x: f32,
    gap_y: i16,
    scored: bool,
}

struct FlappyGame {
    state: GameState,
    bird_y: f32,
    bird_vy: f32,
    score: u32,
    high_score: u32,
    pipes: Vec<PipePair>,
    last_spawn: Instant,
    last_frame: Instant,
}

impl FlappyGame {
    fn new() -> Self {
        Self {
            state: GameState::Ready,
            bird_y: (PLAYABLE_HEIGHT / 2) as f32,
            bird_vy: 0.0,
            score: 0,
            high_score: 0,
            pipes: Vec::new(),
            last_spawn: Instant::now(),
            last_frame: Instant::now(),
        }
    }

    fn reset(&mut self) {
        self.state = GameState::Ready;
        self.bird_y = (PLAYABLE_HEIGHT / 2) as f32;
        self.bird_vy = 0.0;
        self.score = 0;
        self.pipes.clear();
        self.last_spawn = Instant::now();
        self.last_frame = Instant::now();
    }

    fn flap(&mut self) {
        match self.state {
            GameState::Ready => {
                self.state = GameState::Playing;
                self.bird_vy = FLAP_STRENGTH;
            }
            GameState::Playing => {
                self.bird_vy = FLAP_STRENGTH;
            }
            GameState::GameOver => {
                self.reset();
            }
        }
    }

    fn die(&mut self) {
        self.state = GameState::GameOver;
        if self.score > self.high_score {
            self.high_score = self.score;
        }
    }

    fn update(&mut self) {
        let now = Instant::now();
        let dt = now.duration_since(self.last_frame).as_secs_f32();
        self.last_frame = now;

        match self.state {
            GameState::Ready => {
                // Idle gentle floating
                self.bird_y = (PLAYABLE_HEIGHT / 2) as f32;
            }
            GameState::Playing => {
                // Physics integration
                self.bird_vy += GRAVITY;
                if self.bird_vy > MAX_FALL_SPEED {
                    self.bird_vy = MAX_FALL_SPEED;
                }
                self.bird_y += self.bird_vy;

                // Ceiling clamp
                if self.bird_y < 1.0 {
                    self.bird_y = 1.0;
                    self.bird_vy = 0.0;
                }

                // Floor collision
                if self.bird_y >= (PLAYABLE_HEIGHT - 1) as f32 {
                    self.bird_y = (PLAYABLE_HEIGHT - 1) as f32;
                    self.die();
                    return;
                }

                // Spawn pipes
                if self.last_spawn.elapsed().as_secs_f32() >= SPAWN_INTERVAL_SECS {
                    self.last_spawn = Instant::now();
                    // Simple pseudo-random gap offset in Rust
                    let seed = (self.score * 7 + (now.elapsed().as_millis() as u32 % 9)) % 7;
                    let gap_y = 4 + seed as i16;

                    self.pipes.push(PipePair {
                        x: SCREEN_WIDTH as f32,
                        gap_y,
                        scored: false,
                    });
                }

                // Update pipes & AABB collision checks
                let bird_x = 10i16;
                let bird_y_int = self.bird_y.round() as i16;

                for pipe in &mut self.pipes {
                    pipe.x -= PIPE_SPEED;

                    let px = pipe.x.round() as i16;
                    let half_gap = PIPE_GAP_SIZE / 2;
                    let top_pipe_bottom = pipe.gap_y - half_gap;
                    let bottom_pipe_top = pipe.gap_y + half_gap;

                    // Score trigger
                    if !pipe.scored && px + PIPE_WIDTH < bird_x {
                        pipe.scored = true;
                        self.score += 1;
                    }

                    // AABB Collision check
                    if bird_x >= px && bird_x < px + PIPE_WIDTH {
                        if bird_y_int <= top_pipe_bottom || bird_y_int >= bottom_pipe_top {
                            self.die();
                            return;
                        }
                    }
                }

                // Clean off-screen pipes
                self.pipes.retain(|p| p.x > -(PIPE_WIDTH as f32));
            }
            GameState::GameOver => {
                if self.bird_y < (PLAYABLE_HEIGHT - 1) as f32 {
                    self.bird_vy += GRAVITY;
                    self.bird_y += self.bird_vy;
                } else {
                    self.bird_y = (PLAYABLE_HEIGHT - 1) as f32;
                }
            }
        }
    }

    fn render(&self, stdout: &mut Stdout) -> Result<()> {
        execute!(stdout, MoveTo(0, 0))?;

        // 1. Draw Sky & Clouds
        for y in 0..PLAYABLE_HEIGHT {
            execute!(stdout, MoveTo(0, y), SetBackgroundColor(Color::Rgb { r: 78, g: 192, b: 202 }))?;
            let line = " ".repeat(SCREEN_WIDTH as usize);
            execute!(stdout, Print(line))?;
        }

        // 2. Draw Pipes
        for pipe in &self.pipes {
            let px = pipe.x.round() as i16;
            let half_gap = PIPE_GAP_SIZE / 2;
            let top_bottom = pipe.gap_y - half_gap;
            let bot_top = pipe.gap_y + half_gap;

            for y in 0..PLAYABLE_HEIGHT as i16 {
                if y <= top_bottom || y >= bot_top {
                    for bx in 0..PIPE_WIDTH {
                        let draw_x = px + bx;
                        if draw_x >= 0 && draw_x < SCREEN_WIDTH as i16 {
                            execute!(
                                stdout,
                                MoveTo(draw_x as u16, y as u16),
                                SetBackgroundColor(Color::Rgb { r: 115, g: 191, b: 46 }),
                                SetForegroundColor(Color::Rgb { r: 85, g: 128, b: 34 }),
                                Print("#")
                            )?;
                        }
                    }
                }
            }
        }

        // 3. Draw Bird
        let bird_x = 10u16;
        let bird_y = self.bird_y.clamp(0.0, (PLAYABLE_HEIGHT - 1) as f32).round() as u16;
        execute!(
            stdout,
            MoveTo(bird_x, bird_y),
            SetBackgroundColor(Color::Rgb { r: 248, g: 231, b: 28 }),
            SetForegroundColor(Color::Black),
            Print("@>")
        )?;

        // 4. Draw Ground
        for y in PLAYABLE_HEIGHT..SCREEN_HEIGHT {
            execute!(stdout, MoveTo(0, y), SetBackgroundColor(Color::Rgb { r: 222, g: 216, b: 149 }))?;
            let line = if y == PLAYABLE_HEIGHT {
                "=".repeat(SCREEN_WIDTH as usize)
            } else {
                " ".repeat(SCREEN_WIDTH as usize)
            };
            execute!(stdout, SetForegroundColor(Color::Rgb { r: 115, g: 191, b: 46 }), Print(line))?;
        }

        // 5. Draw Overlays / HUD
        execute!(stdout, ResetColor)?;
        match self.state {
            GameState::Ready => {
                execute!(
                    stdout,
                    MoveTo(16, 5),
                    SetForegroundColor(Color::Yellow),
                    Print("FLAPPY BIRD (RUST)"),
                    MoveTo(12, 7),
                    SetForegroundColor(Color::White),
                    Print("Press SPACE or UP to Flap"),
                    MoveTo(16, 9),
                    SetForegroundColor(Color::Cyan),
                    Print(format!("High Score: {}", self.high_score)),
                    MoveTo(18, 12),
                    SetForegroundColor(Color::DarkGrey),
                    Print("Press Q to Quit")
                )?;
            }
            GameState::Playing => {
                execute!(
                    stdout,
                    MoveTo(SCREEN_WIDTH / 2 - 2, 2),
                    SetForegroundColor(Color::White),
                    Print(format!("SCORE: {}", self.score))
                )?;
            }
            GameState::GameOver => {
                execute!(
                    stdout,
                    MoveTo(18, 6),
                    SetForegroundColor(Color::Red),
                    Print("GAME OVER!"),
                    MoveTo(16, 8),
                    SetForegroundColor(Color::White),
                    Print(format!("Final Score: {}", self.score)),
                    MoveTo(16, 9),
                    SetForegroundColor(Color::Cyan),
                    Print(format!("High Score:  {}", self.high_score)),
                    MoveTo(13, 12),
                    SetForegroundColor(Color::Yellow),
                    Print("Press SPACE or R to Restart"),
                    MoveTo(18, 14),
                    SetForegroundColor(Color::DarkGrey),
                    Print("Press Q to Quit")
                )?;
            }
        }

        execute!(stdout, ResetColor)?;
        Ok(())
    }
}

fn main() -> Result<()> {
    enable_raw_mode()?;
    let mut stdout = stdout();
    execute!(stdout, EnterAlternateScreen, Hide, Clear(ClearType::All))?;

    let mut game = FlappyGame::new();
    let frame_duration = Duration::from_millis(33); // ~30 FPS

    'game_loop: loop {
        let frame_start = Instant::now();

        // Process Input
        while event::poll(Duration::from_millis(0))? {
            if let Event::Key(key) = event::read()? {
                if key.kind == KeyEventKind::Press {
                    match key.code {
                        KeyCode::Char('q') | KeyCode::Char('Q') => break 'game_loop,
                        KeyCode::Char(' ') | KeyCode::Up | KeyCode::Char('w') | KeyCode::Char('W') => {
                            game.flap();
                        }
                        KeyCode::Char('r') | KeyCode::Char('R') => {
                            if game.state == GameState::GameOver {
                                game.reset();
                            }
                        }
                        _ => {}
                    }
                }
            }
        }

        // Update Physics & World
        game.update();

        // Render Frame
        game.render(&mut stdout)?;

        // Maintain Target Framerate
        let elapsed = frame_start.elapsed();
        if elapsed < frame_duration {
            std::thread::sleep(frame_duration - elapsed);
        }
    }

    // Cleanup Terminal
    execute!(stdout, Show, LeaveAlternateScreen)?;
    disable_raw_mode()?;
    println!("Thanks for playing Flappy Bird in Rust!");
    Ok(())
}
