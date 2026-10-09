//! Tetris in Rust (crossterm)
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

const GRID_COLS: usize = 10;
const GRID_ROWS: usize = 20;

// Tetromino definitions: 7 shapes, 4 rotations, 4 blocks each: (dCol, dRow)
const TETROMINO_SHAPES: [[[(i32, i32); 4]; 4]; 7] = [
    // 0: I (Cyan)
    [
        [(0, 1), (1, 1), (2, 1), (3, 1)],
        [(2, 0), (2, 1), (2, 2), (2, 3)],
        [(0, 2), (1, 2), (2, 2), (3, 2)],
        [(1, 0), (1, 1), (1, 2), (1, 3)],
    ],
    // 1: O (Yellow)
    [
        [(1, 0), (2, 0), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (2, 1)],
    ],
    // 2: T (Purple)
    [
        [(1, 0), (0, 1), (1, 1), (2, 1)],
        [(1, 0), (1, 1), (2, 1), (1, 2)],
        [(0, 1), (1, 1), (2, 1), (1, 2)],
        [(1, 0), (0, 1), (1, 1), (1, 2)],
    ],
    // 3: S (Green)
    [
        [(1, 0), (2, 0), (0, 1), (1, 1)],
        [(1, 0), (1, 1), (2, 1), (2, 2)],
        [(1, 1), (2, 1), (0, 2), (1, 2)],
        [(0, 0), (0, 1), (1, 1), (1, 2)],
    ],
    // 4: Z (Red)
    [
        [(0, 0), (1, 0), (1, 1), (2, 1)],
        [(2, 0), (1, 1), (2, 1), (1, 2)],
        [(0, 1), (1, 1), (1, 2), (2, 2)],
        [(1, 0), (0, 1), (1, 1), (0, 2)],
    ],
    // 5: J (Blue)
    [
        [(0, 0), (0, 1), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (1, 2)],
        [(0, 1), (1, 1), (2, 1), (2, 2)],
        [(1, 0), (1, 1), (0, 2), (1, 2)],
    ],
    // 6: L (Orange)
    [
        [(2, 0), (0, 1), (1, 1), (2, 1)],
        [(1, 0), (1, 1), (1, 2), (2, 2)],
        [(0, 1), (1, 1), (2, 1), (0, 2)],
        [(0, 0), (1, 0), (1, 1), (1, 2)],
    ],
];

const PIECE_COLORS: [Color; 7] = [
    Color::Cyan,
    Color::Yellow,
    Color::Magenta,
    Color::Green,
    Color::Red,
    Color::Blue,
    Color::Rgb { r: 255, g: 145, b: 0 }, // Orange
];

struct TetrisGame {
    grid: [[u8; GRID_COLS]; GRID_ROWS],
    current_piece: usize,
    current_rot: usize,
    current_col: i32,
    current_row: i32,
    next_piece: usize,
    score: u32,
    lines_cleared: u32,
    level: u32,
    is_game_over: bool,
    drop_interval: Duration,
    last_drop: Instant,
}

impl TetrisGame {
    fn new() -> Self {
        let mut game = Self {
            grid: [[0; GRID_COLS]; GRID_ROWS],
            current_piece: 0,
            current_rot: 0,
            current_col: 3,
            current_row: 0,
            next_piece: 1,
            score: 0,
            lines_cleared: 0,
            level: 1,
            is_game_over: false,
            drop_interval: Duration::from_millis(800),
            last_drop: Instant::now(),
        };
        game.restart();
        game
    }

    fn restart(&mut self) {
        self.grid = [[0; GRID_COLS]; GRID_ROWS];
        self.score = 0;
        self.lines_cleared = 0;
        self.level = 1;
        self.is_game_over = false;
        self.drop_interval = Duration::from_millis(800);
        self.last_drop = Instant::now();
        self.current_piece = 0;
        self.next_piece = 1;
        self.spawn_new_piece();
    }

    fn spawn_new_piece(&mut self) {
        self.current_piece = self.next_piece;
        // Pseudo-random next piece in Rust
        let nanos = Instant::now().elapsed().subsec_nanos();
        self.next_piece = ((self.score as u32 + self.lines_cleared + nanos) % 7) as usize;
        self.current_rot = 0;
        self.current_col = 3;
        self.current_row = 0;

        if !self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row) {
            self.is_game_over = true;
        }
    }

    fn is_valid_position(&self, piece: usize, rot: usize, col: i32, row: i32) -> bool {
        for &(dc, dr) in &TETROMINO_SHAPES[piece][rot] {
            let c = col + dc;
            let r = row + dr;

            if c < 0 || c >= GRID_COLS as i32 || r >= GRID_ROWS as i32 {
                return false;
            }
            if r >= 0 && self.grid[r as usize][c as usize] != 0 {
                return false;
            }
        }
        true
    }

    fn move_left(&mut self) {
        if self.is_game_over { return; }
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col - 1, self.current_row) {
            self.current_col -= 1;
        }
    }

    fn move_right(&mut self) {
        if self.is_game_over { return; }
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col + 1, self.current_row) {
            self.current_col += 1;
        }
    }

    fn rotate(&mut self) {
        if self.is_game_over { return; }
        let next_rot = (self.current_rot + 1) % 4;

        // Basic wall kicks
        if self.is_valid_position(self.current_piece, next_rot, self.current_col, self.current_row) {
            self.current_rot = next_rot;
        } else if self.is_valid_position(self.current_piece, next_rot, self.current_col - 1, self.current_row) {
            self.current_col -= 1;
            self.current_rot = next_rot;
        } else if self.is_valid_position(self.current_piece, next_rot, self.current_col + 1, self.current_row) {
            self.current_col += 1;
            self.current_rot = next_rot;
        }
    }

    fn soft_drop(&mut self) {
        if self.is_game_over { return; }
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row + 1) {
            self.current_row += 1;
            self.score += 1;
            self.last_drop = Instant::now();
        } else {
            self.lock_piece();
        }
    }

    fn hard_drop(&mut self) {
        if self.is_game_over { return; }
        let mut dist = 0;
        while self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row + 1) {
            self.current_row += 1;
            dist += 1;
        }
        self.score += dist * 2;
        self.lock_piece();
    }

    fn step_drop(&mut self) {
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row + 1) {
            self.current_row += 1;
        } else {
            self.lock_piece();
        }
    }

    fn lock_piece(&mut self) {
        for &(dc, dr) in &TETROMINO_SHAPES[self.current_piece][self.current_rot] {
            let c = self.current_col + dc;
            let r = self.current_row + dr;
            if r >= 0 && r < GRID_ROWS as i32 && c >= 0 && c < GRID_COLS as i32 {
                self.grid[r as usize][c as usize] = (self.current_piece + 1) as u8;
            }
        }
        self.clear_lines();
        self.spawn_new_piece();
    }

    fn clear_lines(&mut self) {
        let mut cleared = 0;
        let mut r = (GRID_ROWS - 1) as i32;

        while r >= 0 {
            let row_idx = r as usize;
            let is_full = self.grid[row_idx].iter().all(|&cell| cell != 0);

            if is_full {
                cleared += 1;
                // Shift rows down
                for y in (1..=row_idx).rev() {
                    self.grid[y] = self.grid[y - 1];
                }
                self.grid[0] = [0; GRID_COLS];
            } else {
                r -= 1;
            }
        }

        if cleared > 0 {
            self.lines_cleared += cleared;
            let points = [0, 100, 300, 500, 800];
            self.score += points[cleared.min(4) as usize] * self.level;
            self.level = 1 + (self.lines_cleared / 10);

            let speed_ms = (800.0 * 0.85_f32.powi((self.level - 1) as i32)).max(80.0) as u64;
            self.drop_interval = Duration::from_millis(speed_ms);
        }
    }

    fn get_ghost_row(&self) -> i32 {
        let mut ghost = self.current_row;
        while self.is_valid_position(self.current_piece, self.current_rot, self.current_col, ghost + 1) {
            ghost += 1;
        }
        ghost
    }

    fn update(&mut self) {
        if !self.is_game_over && self.last_drop.elapsed() >= self.drop_interval {
            self.last_drop = Instant::now();
            self.step_drop();
        }
    }

    fn render(&self, stdout: &mut Stdout) -> Result<()> {
        execute!(stdout, MoveTo(0, 0))?;

        let board_x = 4u16;
        let board_y = 2u16;

        // 1. Draw Top Border
        execute!(stdout, MoveTo(board_x, board_y - 1), SetForegroundColor(Color::DarkGrey), Print("+--------------------+"))?;

        // 2. Draw Board Rows
        let ghost_r = self.get_ghost_row();

        for r in 0..GRID_ROWS {
            execute!(stdout, MoveTo(board_x, board_y + r as u16), SetForegroundColor(Color::DarkGrey), Print("|"))?;

            for c in 0..GRID_COLS {
                let cell = self.grid[r][c];

                // Check active piece
                let is_active = !self.is_game_over && TETROMINO_SHAPES[self.current_piece][self.current_rot]
                    .iter()
                    .any(|&(dc, dr)| self.current_col + dc == c as i32 && self.current_row + dr == r as i32);

                // Check ghost piece
                let is_ghost = !self.is_game_over && ghost_r != self.current_row && TETROMINO_SHAPES[self.current_piece][self.current_rot]
                    .iter()
                    .any(|&(dc, dr)| self.current_col + dc == c as i32 && ghost_r + dr == r as i32);

                if cell > 0 {
                    let col = PIECE_COLORS[(cell - 1) as usize];
                    execute!(stdout, SetBackgroundColor(col), Print("  "))?;
                } else if is_active {
                    let col = PIECE_COLORS[self.current_piece];
                    execute!(stdout, SetBackgroundColor(col), Print("[]"))?;
                } else if is_ghost {
                    let col = PIECE_COLORS[self.current_piece];
                    execute!(stdout, SetForegroundColor(col), Print("::"))?;
                } else {
                    execute!(stdout, ResetColor, Print(". "))?;
                }
            }

            execute!(stdout, ResetColor, SetForegroundColor(Color::DarkGrey), Print("|"))?;
        }

        // 3. Draw Bottom Border
        execute!(stdout, MoveTo(board_x, board_y + GRID_ROWS as u16), SetForegroundColor(Color::DarkGrey), Print("+--------------------+"))?;

        // 4. Sidebar Stats & Next Piece Preview
        let sx = board_x + 26;
        execute!(
            stdout,
            ResetColor,
            MoveTo(sx, board_y),
            SetForegroundColor(Color::Yellow),
            Print("TETRIS (RUST)"),
            MoveTo(sx, board_y + 2),
            SetForegroundColor(Color::White),
            Print(format!("SCORE: {}", self.score)),
            MoveTo(sx, board_y + 4),
            Print(format!("LINES: {}", self.lines_cleared)),
            MoveTo(sx, board_y + 6),
            Print(format!("LEVEL: {}", self.level)),
            MoveTo(sx, board_y + 9),
            SetForegroundColor(Color::Cyan),
            Print("NEXT PIECE:"),
            MoveTo(sx, board_y + 10),
            SetForegroundColor(Color::DarkGrey),
            Print("+--------+")
        )?;

        // Render Next Piece preview
        for pr in 0..4 {
            execute!(stdout, MoveTo(sx, board_y + 11 + pr as u16), SetForegroundColor(Color::DarkGrey), Print("|"))?;
            for pc in 0..4 {
                let has_block = TETROMINO_SHAPES[self.next_piece][0]
                    .iter()
                    .any(|&(dc, dr)| dc == pc && dr == pr);

                if has_block {
                    let col = PIECE_COLORS[self.next_piece];
                    execute!(stdout, SetBackgroundColor(col), Print("  "))?;
                } else {
                    execute!(stdout, ResetColor, Print("  "))?;
                }
            }
            execute!(stdout, ResetColor, SetForegroundColor(Color::DarkGrey), Print("|"))?;
        }
        execute!(stdout, MoveTo(sx, board_y + 15), SetForegroundColor(Color::DarkGrey), Print("+--------+"))?;

        // 5. Controls Guide
        execute!(
            stdout,
            ResetColor,
            MoveTo(sx, board_y + 17),
            SetForegroundColor(Color::DarkGrey),
            Print("A / D / Left/Right : Move"),
            MoveTo(sx, board_y + 18),
            Print("W / Up Arrow       : Rotate"),
            MoveTo(sx, board_y + 19),
            Print("S / Down Arrow     : Soft Drop"),
            MoveTo(sx, board_y + 20),
            Print("Space / Enter      : Hard Drop"),
            MoveTo(sx, board_y + 21),
            Print("R : Restart | Q : Quit")
        )?;

        // 6. Game Over Banner
        if self.is_game_over {
            execute!(
                stdout,
                MoveTo(board_x + 3, board_y + 8),
                SetBackgroundColor(Color::Red),
                SetForegroundColor(Color::White),
                Print("  GAME OVER!  "),
                MoveTo(board_x + 1, board_y + 10),
                SetBackgroundColor(Color::Black),
                SetForegroundColor(Color::Yellow),
                Print(" Press R to Retry ")
            )?;
        }

        execute!(stdout, ResetColor)?;
        Ok(())
    }
}

fn main() -> Result<()> {
    enable_raw_mode()?;
    let mut stdout = stdout();
    execute!(stdout, EnterAlternateScreen, Hide, Clear(ClearType::All))?;

    let mut game = TetrisGame::new();
    let frame_duration = Duration::from_millis(16); // ~60 FPS

    'game_loop: loop {
        let frame_start = Instant::now();

        // Process Input
        while event::poll(Duration::from_millis(0))? {
            if let Event::Key(key) = event::read()? {
                if key.kind == KeyEventKind::Press {
                    match key.code {
                        KeyCode::Char('q') | KeyCode::Char('Q') => break 'game_loop,
                        KeyCode::Left | KeyCode::Char('a') | KeyCode::Char('A') => game.move_left(),
                        KeyCode::Right | KeyCode::Char('d') | KeyCode::Char('D') => game.move_right(),
                        KeyCode::Up | KeyCode::Char('w') | KeyCode::Char('W') => game.rotate(),
                        KeyCode::Down | KeyCode::Char('s') | KeyCode::Char('S') => game.soft_drop(),
                        KeyCode::Char(' ') | KeyCode::Enter => game.hard_drop(),
                        KeyCode::Char('r') | KeyCode::Char('R') => game.restart(),
                        _ => {}
                    }
                }
            }
        }

        // Update Game
        game.update();

        // Render Frame
        game.render(&mut stdout)?;

        // Maintain Framerate
        let elapsed = frame_start.elapsed();
        if elapsed < frame_duration {
            std::thread::sleep(frame_duration - elapsed);
        }
    }

    // Cleanup Terminal
    execute!(stdout, Show, LeaveAlternateScreen)?;
    disable_raw_mode()?;
    println!("Thanks for playing Tetris in Rust!");
    Ok(())
}
