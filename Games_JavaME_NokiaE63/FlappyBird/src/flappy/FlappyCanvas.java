package flappy;

import javax.microedition.lcdui.*;
import javax.microedition.media.Manager;
import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordStore;
import java.util.Random;
import java.util.Vector;

/**
 * Authentic J2ME Flappy Bird Canvas for Nokia E63 (320x240 Landscape Display).
 * Compatible with MIDP 2.0 / CLDC 1.1 on Symbian S60 3rd Edition.
 */
public class FlappyCanvas extends Canvas implements Runnable {

    // Screen dimensions (Nokia E63 Landscape LCD)
    public static final int SCREEN_W = 320;
    public static final int SCREEN_H = 240;
    public static final int GROUND_H = 34;

    // States
    public static final int STATE_READY = 0;
    public static final int STATE_PLAYING = 1;
    public static final int STATE_GAMEOVER = 2;

    private int state = STATE_READY;

    // Physics
    private static final float GRAVITY = 0.65f;
    private static final float FLAP_STRENGTH = -7.8f;
    private static final float MAX_FALL = 10.0f;

    // Bird
    private static final int BIRD_X = 65;
    private static final int BIRD_RADIUS = 9;
    private float birdY;
    private float birdVy;

    // Pipe Obstacles
    private static final int PIPE_W = 38;
    private static final float PIPE_SPEED = 2.8f;
    private static final int PIPE_GAP = 72;
    private static final int SPAWN_INTERVAL = 55; // frames

    public static class Pipe {
        public float x;
        public int gapY;
        public boolean scored;

        public Pipe(float x, int gapY) {
            this.x = x;
            this.gapY = gapY;
            this.scored = false;
        }
    }

    private final Vector pipes = new Vector();
    private int spawnTimer = 0;
    private final Random rand = new Random();

    // Scoring & Persistence
    private int score = 0;
    private int highScore = 0;
    private static final String RMS_NAME = "FlappyScore";

    // Loop
    private boolean running = false;
    private Thread gameThread;
    private final MIDlet midlet;

    public FlappyCanvas(MIDlet midlet) {
        this.midlet = midlet;
        setFullScreenMode(true);
        loadHighScore();
        resetGame();
    }

    public synchronized void start() {
        if (!running) {
            running = true;
            gameThread = new Thread(this);
            gameThread.start();
        }
    }

    public synchronized void stop() {
        running = false;
        if (gameThread != null) {
            try {
                gameThread.join(300);
            } catch (InterruptedException ignored) {}
            gameThread = null;
        }
    }

    private void resetGame() {
        state = STATE_READY;
        score = 0;
        birdY = (SCREEN_H - GROUND_H) / 2.0f;
        birdVy = 0;
        pipes.removeAllElements();
        spawnTimer = 0;
        repaint();
    }

    private void flap() {
        if (state == STATE_READY) {
            state = STATE_PLAYING;
            birdVy = FLAP_STRENGTH;
            playBeep(72, 40); // C5 tone
        } else if (state == STATE_PLAYING) {
            birdVy = FLAP_STRENGTH;
            playBeep(72, 40);
        } else if (state == STATE_GAMEOVER) {
            resetGame();
        }
    }

    private void die() {
        state = STATE_GAMEOVER;
        playBeep(48, 120); // Low crash tone
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
    }

    private void playBeep(int note, int dur) {
        try {
            Manager.playTone(note, dur, 80);
        } catch (Throwable ignored) {}
    }

    public void run() {
        // Target ~33 FPS (30ms sleep) - optimal for Nokia E63 ARM9 CPU
        while (running) {
            long startTime = System.currentTimeMillis();

            updateGame();
            repaint();
            serviceRepaints();

            long elapsed = System.currentTimeMillis() - startTime;
            long sleepTime = 30 - elapsed;
            if (sleepTime > 0) {
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException ignored) {}
            }
        }
    }

    private void updateGame() {
        int floorY = SCREEN_H - GROUND_H;

        if (state == STATE_READY) {
            // Idle bobbing
            long now = System.currentTimeMillis();
            birdY = ((floorY) / 2.0f) + (float) Math.sin(now * 0.006) * 5.0f;
            return;
        }

        if (state == STATE_GAMEOVER) {
            if (birdY + BIRD_RADIUS < floorY) {
                birdVy += GRAVITY;
                birdY += birdVy;
            } else {
                birdY = floorY - BIRD_RADIUS;
            }
            return;
        }

        // --- PLAYING STATE ---
        birdVy += GRAVITY;
        if (birdVy > MAX_FALL) birdVy = MAX_FALL;
        birdY += birdVy;

        // Ceiling clamp
        if (birdY - BIRD_RADIUS < 0) {
            birdY = BIRD_RADIUS;
            birdVy = 0;
        }

        // Floor collision
        if (birdY + BIRD_RADIUS >= floorY) {
            birdY = floorY - BIRD_RADIUS;
            die();
            return;
        }

        // Spawn Pipes
        spawnTimer++;
        if (spawnTimer >= SPAWN_INTERVAL) {
            spawnTimer = 0;
            int gapY = 55 + rand.nextInt(floorY - 110);
            pipes.addElement(new Pipe(SCREEN_W + 5, gapY));
        }

        // Move & Collide Pipes
        float birdLeft = BIRD_X - BIRD_RADIUS + 2;
        float birdRight = BIRD_X + BIRD_RADIUS - 2;
        float birdTop = birdY - BIRD_RADIUS + 2;
        float birdBottom = birdY + BIRD_RADIUS - 2;

        for (int i = pipes.size() - 1; i >= 0; i--) {
            Pipe p = (Pipe) pipes.elementAt(i);
            p.x -= PIPE_SPEED;

            // Score trigger
            if (!p.scored && p.x + PIPE_W < BIRD_X) {
                p.scored = true;
                score++;
                playBeep(84, 50); // High chime tone
            }

            // AABB Collision
            int topBottom = p.gapY - (PIPE_GAP / 2);
            int botTop = p.gapY + (PIPE_GAP / 2);

            if (birdRight > p.x && birdLeft < p.x + PIPE_W) {
                if (birdTop < topBottom || birdBottom > botTop) {
                    die();
                    return;
                }
            }

            // Off-screen cleanup
            if (p.x + PIPE_W < -10) {
                pipes.removeElementAt(i);
            }
        }
    }

    protected void paint(Graphics g) {
        int floorY = SCREEN_H - GROUND_H;

        // 1. Sky Background (J2ME RGB)
        g.setColor(0x4ec0ca);
        g.fillRect(0, 0, SCREEN_W, floorY);

        // Background Clouds & Hills
        g.setColor(0x70d8e2);
        g.fillArc(30, 90, 80, 50, 0, 180);
        g.fillArc(180, 80, 110, 60, 0, 180);

        // 2. Draw Pipes
        int capH = 14;
        int capOverhang = 3;

        for (int i = 0; i < pipes.size(); i++) {
            Pipe p = (Pipe) pipes.elementAt(i);
            int px = (int) p.x;
            int topBottom = p.gapY - (PIPE_GAP / 2);
            int botTop = p.gapY + (PIPE_GAP / 2);

            // Top Pipe
            g.setColor(0x73bf2e); // Green
            g.fillRect(px, 0, PIPE_W, topBottom - capH);
            g.setColor(0x558022); // Dark border
            g.drawRect(px, 0, PIPE_W, topBottom - capH);

            // Top Pipe Cap
            g.setColor(0x73bf2e);
            g.fillRect(px - capOverhang, topBottom - capH, PIPE_W + capOverhang * 2, capH);
            g.setColor(0x558022);
            g.drawRect(px - capOverhang, topBottom - capH, PIPE_W + capOverhang * 2, capH);

            // Bottom Pipe Cap
            g.setColor(0x73bf2e);
            g.fillRect(px - capOverhang, botTop, PIPE_W + capOverhang * 2, capH);
            g.setColor(0x558022);
            g.drawRect(px - capOverhang, botTop, PIPE_W + capOverhang * 2, capH);

            // Bottom Pipe Body
            g.setColor(0x73bf2e);
            g.fillRect(px, botTop + capH, PIPE_W, floorY - (botTop + capH));
            g.setColor(0x558022);
            g.drawRect(px, botTop + capH, PIPE_W, floorY - (botTop + capH));
        }

        // 3. Ground
        g.setColor(0xded895);
        g.fillRect(0, floorY, SCREEN_W, GROUND_H);
        g.setColor(0x73bf2e); // Grass strip
        g.fillRect(0, floorY, SCREEN_W, 6);
        g.setColor(0x558022);
        g.drawLine(0, floorY, SCREEN_W, floorY);

        // 4. Bird
        int bx = BIRD_X;
        int by = (int) birdY;
        int r = BIRD_RADIUS;

        // Body
        g.setColor(0xf8e71c); // Yellow
        g.fillArc(bx - r, by - r, r * 2, r * 2, 0, 360);
        g.setColor(0xd08b00);
        g.drawArc(bx - r, by - r, r * 2, r * 2, 0, 360);

        // Wing
        g.setColor(0xffffff);
        g.fillArc(bx - r + 2, by - 2, 8, 6, 0, 360);

        // Eye
        g.setColor(0xffffff);
        g.fillArc(bx + 1, by - 6, 6, 6, 0, 360);
        g.setColor(0x000000);
        g.fillArc(bx + 4, by - 4, 3, 3, 0, 360);

        // Beak
        g.setColor(0xf56b2a);
        g.fillRect(bx + r - 2, by - 1, 5, 4);

        // 5. HUD Text
        Font largeFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_LARGE);
        Font mediumFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM);
        Font smallFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL);

        if (state == STATE_READY) {
            g.setFont(largeFont);
            drawShadowString(g, "FLAPPY BIRD", SCREEN_W / 2, 50, Graphics.HCENTER | Graphics.TOP, 0xffffff, 0x223344);

            g.setFont(mediumFont);
            drawShadowString(g, "Press NAVI / SPACE to Flap", SCREEN_W / 2, 90, Graphics.HCENTER | Graphics.TOP, 0xf8e71c, 0x223344);

            g.setFont(smallFont);
            drawShadowString(g, "Nokia E63 Edition (320x240)", SCREEN_W / 2, 120, Graphics.HCENTER | Graphics.TOP, 0xffffff, 0x223344);
            drawShadowString(g, "High Score: " + highScore, SCREEN_W / 2, 145, Graphics.HCENTER | Graphics.TOP, 0xffffff, 0x223344);
        } else if (state == STATE_PLAYING) {
            g.setFont(largeFont);
            drawShadowString(g, String.valueOf(score), SCREEN_W / 2, 18, Graphics.HCENTER | Graphics.TOP, 0xffffff, 0x112233);
        } else if (state == STATE_GAMEOVER) {
            g.setFont(largeFont);
            drawShadowString(g, "GAME OVER", SCREEN_W / 2, 45, Graphics.HCENTER | Graphics.TOP, 0xe74c3c, 0x222222);

            g.setFont(mediumFont);
            drawShadowString(g, "Score: " + score, SCREEN_W / 2, 85, Graphics.HCENTER | Graphics.TOP, 0xffffff, 0x222222);
            drawShadowString(g, "Best: " + highScore, SCREEN_W / 2, 110, Graphics.HCENTER | Graphics.TOP, 0xf8e71c, 0x222222);

            g.setFont(smallFont);
            drawShadowString(g, "Press NAVI / SPACE / R to Restart", SCREEN_W / 2, 150, Graphics.HCENTER | Graphics.TOP, 0xffffff, 0x222222);
        }
    }

    private void drawShadowString(Graphics g, String s, int x, int y, int anchor, int color, int shadowColor) {
        g.setColor(shadowColor);
        g.drawString(s, x + 1, y + 1, anchor);
        g.setColor(color);
        g.drawString(s, x, y, anchor);
    }

    protected void keyPressed(int keyCode) {
        int action = getGameAction(keyCode);
        if (action == FIRE || action == UP || keyCode == 32 || keyCode == 'w' || keyCode == 'W') {
            flap();
        } else if (keyCode == 'r' || keyCode == 'R') {
            if (state == STATE_GAMEOVER) resetGame();
        } else if (keyCode == NOKIA_KEY_SOFTKEY_RIGHT) {
            midlet.notifyDestroyed();
        }
    }

    // --- RMS High Score Persistence ---
    private void loadHighScore() {
        try {
            RecordStore rs = RecordStore.openRecordStore(RMS_NAME, true);
            if (rs.getNumRecords() > 0) {
                byte[] b = rs.getRecord(1);
                highScore = ((b[0] & 0xFF) << 8) | (b[1] & 0xFF);
            }
            rs.closeRecordStore();
        } catch (Throwable ignored) {}
    }

    private void saveHighScore() {
        try {
            RecordStore rs = RecordStore.openRecordStore(RMS_NAME, true);
            byte[] b = new byte[]{(byte) ((highScore >> 8) & 0xFF), (byte) (highScore & 0xFF)};
            if (rs.getNumRecords() > 0) {
                rs.setRecord(1, b, 0, b.length);
            } else {
                rs.addRecord(b, 0, b.length);
            }
            rs.closeRecordStore();
        } catch (Throwable ignored) {}
    }
}
