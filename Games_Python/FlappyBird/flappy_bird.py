"""
Flappy Bird in Python (Standard Tkinter)
Learning Implementation comparing Unreal Engine C++, Unity C#, Python, and Java.
No external dependencies required (runs with standard python flappy_bird.py).
"""

import tkinter as tk
import random
import time

# --- Game Configuration Constants ---
SCREEN_WIDTH = 450
SCREEN_HEIGHT = 650
FPS = 60
FRAME_MS = int(1000 / FPS)

# Physics
GRAVITY = 0.55
FLAP_STRENGTH = -9.2
MAX_FALL_SPEED = 14.0

# Pipes
PIPE_WIDTH = 64
PIPE_SPEED = 3.2
PIPE_SPAWN_INTERVAL = 110  # frames (~1.8s at 60 FPS)
GAP_SIZE = 160
MIN_GAP_Y = 120
MAX_GAP_Y = 460

# World
GROUND_HEIGHT = 80
CEILING_Y = 0
BIRD_X = 110
BIRD_RADIUS = 18


class FlappyBirdGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Flappy Bird (Python / Tkinter)")
        self.root.resizable(False, False)

        # Main Canvas
        self.canvas = tk.Canvas(
            root,
            width=SCREEN_WIDTH,
            height=SCREEN_HEIGHT,
            bg="#4ec0ca",  # Classic Flappy sky blue
            highlightthickness=0
        )
        self.canvas.pack()

        # State: "READY", "PLAYING", "GAMEOVER"
        self.state = "READY"
        self.score = 0
        self.high_score = 0

        # Bird physics state
        self.bird_y = SCREEN_HEIGHT // 2
        self.bird_vy = 0.0

        # Pipe list: list of dicts: {"x": float, "gap_y": float, "scored": bool}
        self.pipes = []
        self.spawn_counter = 0

        # Bindings
        self.root.bind("<space>", self.on_key_flap)
        self.root.bind("<Up>", self.on_key_flap)
        self.root.bind("<w>", self.on_key_flap)
        self.root.bind("<Button-1>", self.on_click)
        self.root.bind("<r>", self.on_key_restart)

        # Visual item IDs
        self.bird_body_id = None
        self.bird_eye_id = None
        self.bird_pupil_id = None
        self.bird_beak_id = None
        self.bird_wing_id = None

        self.reset_game()
        self.root.after(FRAME_MS, self.game_loop)

    def reset_game(self):
        self.state = "READY"
        self.score = 0
        self.bird_y = (SCREEN_HEIGHT - GROUND_HEIGHT) // 2
        self.bird_vy = 0.0
        self.pipes.clear()
        self.spawn_counter = 0

    def start_game(self):
        if self.state == "READY":
            self.state = "PLAYING"
            self.flap()

    def flap(self):
        if self.state == "PLAYING":
            self.bird_vy = FLAP_STRENGTH

    def on_key_flap(self, event):
        if self.state == "READY":
            self.start_game()
        elif self.state == "PLAYING":
            self.flap()
        elif self.state == "GAMEOVER":
            self.reset_game()

    def on_click(self, event):
        self.on_key_flap(event)

    def on_key_restart(self, event):
        if self.state == "GAMEOVER":
            self.reset_game()

    def die(self):
        self.state = "GAMEOVER"
        if self.score > self.high_score:
            self.high_score = self.score

    def game_loop(self):
        self.update_simulation()
        self.render()
        self.root.after(FRAME_MS, self.game_loop)

    def update_simulation(self):
        playable_floor = SCREEN_HEIGHT - GROUND_HEIGHT

        if self.state == "READY":
            # Gentle hover bobbing
            t = time.time() * 5.0
            self.bird_y = ((SCREEN_HEIGHT - GROUND_HEIGHT) // 2) + int(5.0 * random.random() * 0.2 + 8.0 * (time.time() % 1.0 - 0.5))
            return

        if self.state == "GAMEOVER":
            # Bird tumbles to ground
            if self.bird_y + BIRD_RADIUS < playable_floor:
                self.bird_vy += GRAVITY
                self.bird_y += self.bird_vy
            else:
                self.bird_y = playable_floor - BIRD_RADIUS
            return

        # --- PLAYING STATE ---
        # 1. Physics
        self.bird_vy += GRAVITY
        if self.bird_vy > MAX_FALL_SPEED:
            self.bird_vy = MAX_FALL_SPEED

        self.bird_y += self.bird_vy

        # Ceiling check
        if self.bird_y - BIRD_RADIUS < CEILING_Y:
            self.bird_y = CEILING_Y + BIRD_RADIUS
            self.bird_vy = 0

        # Floor check
        if self.bird_y + BIRD_RADIUS >= playable_floor:
            self.bird_y = playable_floor - BIRD_RADIUS
            self.die()
            return

        # 2. Pipes Spawning
        self.spawn_counter += 1
        if self.spawn_counter >= PIPE_SPAWN_INTERVAL:
            self.spawn_counter = 0
            gap_y = random.randint(MIN_GAP_Y, MAX_GAP_Y)
            self.pipes.append({"x": SCREEN_WIDTH + 10, "gap_y": gap_y, "scored": False})

        # 3. Pipes Movement & Collision
        bird_left = BIRD_X - BIRD_RADIUS + 4
        bird_right = BIRD_X + BIRD_RADIUS - 4
        bird_top = self.bird_y - BIRD_RADIUS + 4
        bird_bottom = self.bird_y + BIRD_RADIUS - 4

        for pipe in self.pipes:
            pipe["x"] -= PIPE_SPEED
            px = pipe["x"]
            gap_y = pipe["gap_y"]
            top_pipe_bottom = gap_y - (GAP_SIZE // 2)
            bot_pipe_top = gap_y + (GAP_SIZE // 2)

            # Score trigger
            if not pipe["scored"] and px + PIPE_WIDTH < BIRD_X:
                pipe["scored"] = True
                self.score += 1

            # AABB Collision Check
            pipe_right = px + PIPE_WIDTH
            if bird_right > px and bird_left < pipe_right:
                # Top pipe collision
                if bird_top < top_pipe_bottom:
                    self.die()
                    return
                # Bottom pipe collision
                if bird_bottom > bot_pipe_top:
                    self.die()
                    return

        # Clean off-screen pipes
        self.pipes = [p for p in self.pipes if p["x"] + PIPE_WIDTH > -20]

    def render(self):
        self.canvas.delete("all")

        # 1. Clouds / Background
        self.canvas.create_oval(60, 100, 180, 150, fill="#ffffff", outline="")
        self.canvas.create_oval(250, 80, 390, 130, fill="#ffffff", outline="")

        # 2. Draw Pipes
        pipe_fill = "#73bf2e"
        pipe_border = "#558022"
        cap_height = 24
        cap_overhang = 5

        playable_floor = SCREEN_HEIGHT - GROUND_HEIGHT

        for pipe in self.pipes:
            px = pipe["x"]
            gap_y = pipe["gap_y"]
            top_pipe_bottom = gap_y - (GAP_SIZE // 2)
            bot_pipe_top = gap_y + (GAP_SIZE // 2)

            # Top Pipe Body
            self.canvas.create_rectangle(
                px, 0, px + PIPE_WIDTH, top_pipe_bottom - cap_height,
                fill=pipe_fill, outline=pipe_border, width=2
            )
            # Top Pipe Cap
            self.canvas.create_rectangle(
                px - cap_overhang, top_pipe_bottom - cap_height,
                px + PIPE_WIDTH + cap_overhang, top_pipe_bottom,
                fill=pipe_fill, outline=pipe_border, width=2
            )

            # Bottom Pipe Cap
            self.canvas.create_rectangle(
                px - cap_overhang, bot_pipe_top,
                px + PIPE_WIDTH + cap_overhang, bot_pipe_top + cap_height,
                fill=pipe_fill, outline=pipe_border, width=2
            )
            # Bottom Pipe Body
            self.canvas.create_rectangle(
                px, bot_pipe_top + cap_height,
                px + PIPE_WIDTH, playable_floor,
                fill=pipe_fill, outline=pipe_border, width=2
            )

        # 3. Ground
        self.canvas.create_rectangle(
            0, playable_floor, SCREEN_WIDTH, SCREEN_HEIGHT,
            fill="#ded895", outline="#73bf2e", width=4
        )
        # Ground stripe
        self.canvas.create_line(
            0, playable_floor + 12, SCREEN_WIDTH, playable_floor + 12,
            fill="#c8be6b", width=3
        )

        # 4. Bird
        bx = BIRD_X
        by = int(self.bird_y)
        r = BIRD_RADIUS

        # Body
        self.canvas.create_oval(bx - r, by - r, bx + r, by + r, fill="#f8e71c", outline="#d08b00", width=2)
        # Wing
        wing_y_offset = -2 if self.bird_vy < 0 else 3
        self.canvas.create_oval(bx - r + 3, by + wing_y_offset - 4, bx + 2, by + wing_y_offset + 6, fill="#ffffff", outline="#d08b00", width=1)
        # Eye
        self.canvas.create_oval(bx + 3, by - 10, bx + 13, by, fill="#ffffff", outline="#000000", width=1)
        self.canvas.create_oval(bx + 8, by - 7, bx + 12, by - 3, fill="#000000", outline="")
        # Beak
        self.canvas.create_polygon(bx + 10, by - 2, bx + 20, by + 2, bx + 10, by + 6, fill="#f56b2a", outline="#b03e08")

        # 5. UI Overlays
        if self.state == "READY":
            self.draw_shadow_text(SCREEN_WIDTH // 2, 180, "FLAPPY BIRD", font=("Arial", 32, "bold"), fill="#ffffff")
            self.draw_shadow_text(SCREEN_WIDTH // 2, 250, "Press SPACE or CLICK to Flap", font=("Arial", 16, "bold"), fill="#f8e71c")
            self.draw_shadow_text(SCREEN_WIDTH // 2, 310, f"High Score: {self.high_score}", font=("Arial", 14, "normal"), fill="#ffffff")

        elif self.state == "PLAYING":
            self.draw_shadow_text(SCREEN_WIDTH // 2, 60, str(self.score), font=("Arial", 36, "bold"), fill="#ffffff")

        elif self.state == "GAMEOVER":
            self.draw_shadow_text(SCREEN_WIDTH // 2, 170, "GAME OVER", font=("Arial", 34, "bold"), fill="#e74c3c")
            self.draw_shadow_text(SCREEN_WIDTH // 2, 240, f"Score: {self.score}", font=("Arial", 24, "bold"), fill="#ffffff")
            self.draw_shadow_text(SCREEN_WIDTH // 2, 285, f"Best: {self.high_score}", font=("Arial", 18, "normal"), fill="#f8e71c")
            self.draw_shadow_text(SCREEN_WIDTH // 2, 350, "Press R or SPACE to Restart", font=("Arial", 16, "bold"), fill="#ffffff")

    def draw_shadow_text(self, x, y, text, font, fill="#ffffff"):
        # Shadow
        self.canvas.create_text(x + 2, y + 2, text=text, font=font, fill="#2c3e50")
        # Main text
        self.canvas.create_text(x, y, text=text, font=font, fill=fill)


if __name__ == "__main__":
    tk_root = tk.Tk()
    app = FlappyBirdGame(tk_root)
    tk_root.mainloop()
