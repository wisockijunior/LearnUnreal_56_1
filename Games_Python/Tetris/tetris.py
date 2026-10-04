"""
Tetris in Python (Standard Tkinter)
Learning Implementation comparing Unreal Engine C++, Unity C#, Python, and Java.
No external dependencies required (runs with standard python tetris.py).
"""

import tkinter as tk
import random
import time

GRID_COLS = 10
GRID_ROWS = 20
CELL_SIZE = 28  # Pixels per cell

BOARD_PIXEL_W = GRID_COLS * CELL_SIZE
BOARD_PIXEL_H = GRID_ROWS * CELL_SIZE
SIDEBAR_W = 180

WINDOW_WIDTH = BOARD_PIXEL_W + SIDEBAR_W + 30
WINDOW_HEIGHT = BOARD_PIXEL_H + 40

# Tetromino shapes: 7 pieces, 4 rotations, 4 blocks each (col, row relative coordinates)
TETROMINO_SHAPES = [
    # 0: I (Cyan)
    [
        [(0, 1), (1, 1), (2, 1), (3, 1)],
        [(2, 0), (2, 1), (2, 2), (2, 3)],
        [(0, 2), (1, 2), (2, 2), (3, 2)],
        [(1, 0), (1, 1), (1, 2), (1, 3)]
    ],
    # 1: O (Yellow)
    [
        [(1, 0), (2, 0), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (2, 1)]
    ],
    # 2: T (Purple)
    [
        [(1, 0), (0, 1), (1, 1), (2, 1)],
        [(1, 0), (1, 1), (2, 1), (1, 2)],
        [(0, 1), (1, 1), (2, 1), (1, 2)],
        [(1, 0), (0, 1), (1, 1), (1, 2)]
    ],
    # 3: S (Green)
    [
        [(1, 0), (2, 0), (0, 1), (1, 1)],
        [(1, 0), (1, 1), (2, 1), (2, 2)],
        [(1, 1), (2, 1), (0, 2), (1, 2)],
        [(0, 0), (0, 1), (1, 1), (1, 2)]
    ],
    # 4: Z (Red)
    [
        [(0, 0), (1, 0), (1, 1), (2, 1)],
        [(2, 0), (1, 1), (2, 1), (1, 2)],
        [(0, 1), (1, 1), (1, 2), (2, 2)],
        [(1, 0), (0, 1), (1, 1), (0, 2)]
    ],
    # 5: J (Blue)
    [
        [(0, 0), (0, 1), (1, 1), (2, 1)],
        [(1, 0), (2, 0), (1, 1), (1, 2)],
        [(0, 1), (1, 1), (2, 1), (2, 2)],
        [(1, 0), (1, 1), (0, 2), (1, 2)]
    ],
    # 6: L (Orange)
    [
        [(2, 0), (0, 1), (1, 1), (2, 1)],
        [(1, 0), (1, 1), (1, 2), (2, 2)],
        [(0, 1), (1, 1), (2, 1), (0, 2)],
        [(0, 0), (1, 0), (1, 1), (1, 2)]
    ]
]

PIECE_COLORS = [
    "#00e5ff",  # Cyan (I)
    "#ffea00",  # Yellow (O)
    "#d500f9",  # Purple (T)
    "#00e676",  # Green (S)
    "#ff1744",  # Red (Z)
    "#2979ff",  # Blue (J)
    "#ff9100"   # Orange (L)
]

DARK_PIECE_COLORS = [
    "#007788", "#887700", "#660077", "#007733", "#880011", "#113388", "#884400"
]


class TetrisGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Tetris (Python / Tkinter)")
        self.root.resizable(False, False)

        self.canvas = tk.Canvas(
            root,
            width=WINDOW_WIDTH,
            height=WINDOW_HEIGHT,
            bg="#11151c",
            highlightthickness=0
        )
        self.canvas.pack()

        # Grid state: 0 = empty, 1..7 = piece color
        self.grid = [[0 for _ in range(GRID_COLS)] for _ in range(GRID_ROWS)]

        # Falling piece
        self.current_piece = 0
        self.current_rot = 0
        self.current_col = 3
        self.current_row = 0
        self.next_piece = random.randint(0, 6)

        # Progression
        self.score = 0
        self.lines_cleared = 0
        self.level = 1
        self.is_game_over = False

        self.base_drop_interval = 0.8
        self.current_drop_interval = self.base_drop_interval
        self.last_drop_time = time.time()

        # Input bindings
        self.root.bind("<Left>", lambda e: self.move_left())
        self.root.bind("<a>", lambda e: self.move_left())
        self.root.bind("<Right>", lambda e: self.move_right())
        self.root.bind("<d>", lambda e: self.move_right())
        self.root.bind("<Up>", lambda e: self.rotate_piece())
        self.root.bind("<w>", lambda e: self.rotate_piece())
        self.root.bind("<Down>", lambda e: self.soft_drop())
        self.root.bind("<s>", lambda e: self.soft_drop())
        self.root.bind("<space>", lambda e: self.hard_drop())
        self.root.bind("<Return>", lambda e: self.hard_drop())
        self.root.bind("<r>", lambda e: self.restart())

        self.board_origin_x = 20
        self.board_origin_y = 20

        self.restart()
        self.game_loop()

    def restart(self):
        self.grid = [[0 for _ in range(GRID_COLS)] for _ in range(GRID_ROWS)]
        self.score = 0
        self.lines_cleared = 0
        self.level = 1
        self.is_game_over = False
        self.current_drop_interval = self.base_drop_interval
        self.next_piece = random.randint(0, 6)
        self.spawn_new_piece()

    def spawn_new_piece(self):
        self.current_piece = self.next_piece
        self.next_piece = random.randint(0, 6)
        self.current_rot = 0
        self.current_col = 3
        self.current_row = 0

        if not self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row):
            self.is_game_over = True

    def is_valid_position(self, piece_type, rot, col, row):
        blocks = TETROMINO_SHAPES[piece_type][rot]
        for dc, dr in blocks:
            c = col + dc
            r = row + dr
            if c < 0 or c >= GRID_COLS or r >= GRID_ROWS:
                return False
            if r >= 0 and self.grid[r][c] != 0:
                return False
        return True

    def move_left(self):
        if self.is_game_over: return
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col - 1, self.current_row):
            self.current_col -= 1

    def move_right(self):
        if self.is_game_over: return
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col + 1, self.current_row):
            self.current_col += 1

    def rotate_piece(self):
        if self.is_game_over: return
        next_rot = (self.current_rot + 1) % 4
        # Standard wall kick tests
        if self.is_valid_position(self.current_piece, next_rot, self.current_col, self.current_row):
            self.current_rot = next_rot
        elif self.is_valid_position(self.current_piece, next_rot, self.current_col - 1, self.current_row):
            self.current_col -= 1
            self.current_rot = next_rot
        elif self.is_valid_position(self.current_piece, next_rot, self.current_col + 1, self.current_row):
            self.current_col += 1
            self.current_rot = next_rot

    def soft_drop(self):
        if self.is_game_over: return
        if self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row + 1):
            self.current_row += 1
            self.score += 1
            self.last_drop_time = time.time()
        else:
            self.lock_piece()

    def hard_drop(self):
        if self.is_game_over: return
        drop_dist = 0
        while self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row + 1):
            self.current_row += 1
            drop_dist += 1
        self.score += drop_dist * 2
        self.lock_piece()

    def lock_piece(self):
        blocks = TETROMINO_SHAPES[self.current_piece][self.current_rot]
        for dc, dr in blocks:
            c = self.current_col + dc
            r = self.current_row + dr
            if 0 <= r < GRID_ROWS and 0 <= c < GRID_COLS:
                self.grid[r][c] = self.current_piece + 1

        self.clear_lines()
        self.spawn_new_piece()

    def clear_lines(self):
        lines_to_clear = []
        for r in range(GRID_ROWS):
            if all(self.grid[r][c] != 0 for c in range(GRID_COLS)):
                lines_to_clear.append(r)

        cleared_count = len(lines_to_clear)
        if cleared_count > 0:
            for r in lines_to_clear:
                del self.grid[r]
                self.grid.insert(0, [0 for _ in range(GRID_COLS)])

            self.lines_cleared += cleared_count
            points = [0, 100, 300, 500, 800]
            self.score += points[min(cleared_count, 4)] * self.level
            self.level = 1 + (self.lines_cleared // 10)
            self.current_drop_interval = max(0.08, self.base_drop_interval * (0.85 ** (self.level - 1)))

    def game_loop(self):
        now = time.time()
        if not self.is_game_over:
            if now - self.last_drop_time >= self.current_drop_interval:
                self.last_drop_time = now
                if self.is_valid_position(self.current_piece, self.current_rot, self.current_col, self.current_row + 1):
                    self.current_row += 1
                else:
                    self.lock_piece()

        self.render()
        self.root.after(16, self.game_loop)

    def get_ghost_row(self):
        ghost_r = self.current_row
        while self.is_valid_position(self.current_piece, self.current_rot, self.current_col, ghost_r + 1):
            ghost_r += 1
        return ghost_r

    def render(self):
        self.canvas.delete("all")
        bx = self.board_origin_x
        by = self.board_origin_y

        # 1. Background Grid Playfield
        self.canvas.create_rectangle(
            bx, by, bx + BOARD_PIXEL_W, by + BOARD_PIXEL_H,
            fill="#1e2430", outline="#3a4454", width=2
        )

        # Subtle cell grid lines
        for r in range(1, GRID_ROWS):
            y = by + r * CELL_SIZE
            self.canvas.create_line(bx, y, bx + BOARD_PIXEL_W, y, fill="#252c3b", width=1)
        for c in range(1, GRID_COLS):
            x = bx + c * CELL_SIZE
            self.canvas.create_line(x, by, x, by + BOARD_PIXEL_H, fill="#252c3b", width=1)

        # 2. Draw Locked Blocks
        for r in range(GRID_ROWS):
            for c in range(GRID_COLS):
                val = self.grid[r][c]
                if val > 0:
                    self.draw_cell(bx + c * CELL_SIZE, by + r * CELL_SIZE, PIECE_COLORS[val - 1])

        # 3. Draw Ghost Piece (projection)
        if not self.is_game_over:
            ghost_r = self.get_ghost_row()
            if ghost_r != self.current_row:
                for dc, dr in TETROMINO_SHAPES[self.current_piece][self.current_rot]:
                    c = self.current_col + dc
                    r = ghost_r + dr
                    if r >= 0:
                        x = bx + c * CELL_SIZE
                        y = by + r * CELL_SIZE
                        self.canvas.create_rectangle(
                            x + 2, y + 2, x + CELL_SIZE - 2, y + CELL_SIZE - 2,
                            outline=PIECE_COLORS[self.current_piece], width=1, dash=(2, 2)
                        )

        # 4. Draw Active Falling Piece
        if not self.is_game_over:
            for dc, dr in TETROMINO_SHAPES[self.current_piece][self.current_rot]:
                c = self.current_col + dc
                r = self.current_row + dr
                if r >= 0:
                    self.draw_cell(bx + c * CELL_SIZE, by + r * CELL_SIZE, PIECE_COLORS[self.current_piece])

        # 5. Sidebar - Next Piece Preview
        sx = bx + BOARD_PIXEL_W + 20
        sy = by

        self.canvas.create_text(sx, sy + 10, text="NEXT PIECE", font=("Arial", 12, "bold"), fill="#8fa1b8", anchor="w")
        self.canvas.create_rectangle(sx, sy + 30, sx + 130, sy + 130, fill="#181e29", outline="#3a4454", width=1)

        next_blocks = TETROMINO_SHAPES[self.next_piece][0]
        preview_box_center_x = sx + 65
        preview_box_center_y = sy + 80
        # Calculate bounds to center piece in box
        min_c = min(b[0] for b in next_blocks)
        max_c = max(b[0] for b in next_blocks)
        min_r = min(b[1] for b in next_blocks)
        max_r = max(b[1] for b in next_blocks)
        off_x = preview_box_center_x - ((max_c - min_c + 1) * CELL_SIZE // 2) - (min_c * CELL_SIZE)
        off_y = preview_box_center_y - ((max_r - min_r + 1) * CELL_SIZE // 2) - (min_r * CELL_SIZE)

        for dc, dr in next_blocks:
            self.draw_cell(off_x + dc * CELL_SIZE, off_y + dr * CELL_SIZE, PIECE_COLORS[self.next_piece])

        # 6. Sidebar - Score, Lines, Level
        info_y = sy + 160
        self.canvas.create_text(sx, info_y, text="SCORE", font=("Arial", 11, "bold"), fill="#8fa1b8", anchor="w")
        self.canvas.create_text(sx, info_y + 22, text=str(self.score), font=("Arial", 16, "bold"), fill="#ffffff", anchor="w")

        info_y += 55
        self.canvas.create_text(sx, info_y, text="LINES", font=("Arial", 11, "bold"), fill="#8fa1b8", anchor="w")
        self.canvas.create_text(sx, info_y + 22, text=str(self.lines_cleared), font=("Arial", 16, "bold"), fill="#ffffff", anchor="w")

        info_y += 55
        self.canvas.create_text(sx, info_y, text="LEVEL", font=("Arial", 11, "bold"), fill="#8fa1b8", anchor="w")
        self.canvas.create_text(sx, info_y + 22, text=str(self.level), font=("Arial", 16, "bold"), fill="#ffd700", anchor="w")

        # 7. Sidebar - Controls
        info_y += 70
        self.canvas.create_rectangle(sx, info_y, sx + 130, info_y + 160, fill="#181e29", outline="#3a4454", width=1)
        self.canvas.create_text(sx + 10, info_y + 15, text="CONTROLS", font=("Arial", 10, "bold"), fill="#8fa1b8", anchor="w")
        self.canvas.create_text(sx + 10, info_y + 38, text="A/D : Move", font=("Arial", 9, "normal"), fill="#cccccc", anchor="w")
        self.canvas.create_text(sx + 10, info_y + 60, text="W : Rotate", font=("Arial", 9, "normal"), fill="#cccccc", anchor="w")
        self.canvas.create_text(sx + 10, info_y + 82, text="S : Soft Drop", font=("Arial", 9, "normal"), fill="#cccccc", anchor="w")
        self.canvas.create_text(sx + 10, info_y + 104, text="Space : Hard Drop", font=("Arial", 9, "normal"), fill="#cccccc", anchor="w")
        self.canvas.create_text(sx + 10, info_y + 126, text="R : Restart", font=("Arial", 9, "normal"), fill="#cccccc", anchor="w")

        # 8. Game Over Banner
        if self.is_game_over:
            mid_x = bx + BOARD_PIXEL_W // 2
            mid_y = by + BOARD_PIXEL_H // 2
            self.canvas.create_rectangle(bx + 10, mid_y - 50, bx + BOARD_PIXEL_W - 10, mid_y + 50, fill="#0a0c10", outline="#e74c3c", width=2)
            self.canvas.create_text(mid_x, mid_y - 15, text="GAME OVER", font=("Arial", 22, "bold"), fill="#e74c3c")
            self.canvas.create_text(mid_x, mid_y + 20, text="Press R to Play Again", font=("Arial", 12, "normal"), fill="#ffffff")

    def draw_cell(self, x, y, color):
        # Beveled block rendering
        self.canvas.create_rectangle(x + 1, y + 1, x + CELL_SIZE - 1, y + CELL_SIZE - 1, fill=color, outline="")
        # Highlight top/left
        self.canvas.create_line(x + 1, y + 1, x + CELL_SIZE - 1, y + 1, fill="#ffffff", width=1)
        self.canvas.create_line(x + 1, y + 1, x + 1, y + CELL_SIZE - 1, fill="#ffffff", width=1)
        # Shadow bot/right
        self.canvas.create_line(x + 1, y + CELL_SIZE - 1, x + CELL_SIZE - 1, y + CELL_SIZE - 1, fill="#000000", width=1)
        self.canvas.create_line(x + CELL_SIZE - 1, y + 1, x + CELL_SIZE - 1, y + CELL_SIZE - 1, fill="#000000", width=1)


if __name__ == "__main__":
    tk_root = tk.Tk()
    app = TetrisGame(tk_root)
    tk_root.mainloop()
