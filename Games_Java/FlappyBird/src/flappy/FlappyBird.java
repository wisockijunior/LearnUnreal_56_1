package flappy;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

/**
 * Flappy Bird Implementation in Java (Swing / AWT).
 * Learning implementation comparing Unreal C++, Unity C#, Python, and Java.
 * Self-contained, runnable with standard JDK.
 */
public class FlappyBird extends JPanel implements ActionListener, KeyListener, MouseListener {

    public static final int SCREEN_WIDTH = 450;
    public static final int SCREEN_HEIGHT = 650;
    public static final int GROUND_HEIGHT = 80;

    public enum State {
        READY, PLAYING, GAMEOVER
    }

    // Physics
    private static final double GRAVITY = 0.55;
    private static final double FLAP_STRENGTH = -9.2;
    private static final double MAX_FALL_SPEED = 14.0;

    // Bird
    private static final int BIRD_X = 110;
    private static final int BIRD_RADIUS = 18;
    private double birdY;
    private double birdVy;

    // Pipes
    public static class Pipe {
        public double x;
        public int gapY;
        public boolean scored;

        public Pipe(double x, int gapY) {
            this.x = x;
            this.gapY = gapY;
            this.scored = false;
        }
    }

    private static final int PIPE_WIDTH = 64;
    private static final double PIPE_SPEED = 3.2;
    private static final int PIPE_GAP = 160;
    private static final int PIPE_SPAWN_INTERVAL = 110; // frames (~1.8s)

    private final List<Pipe> pipes = new ArrayList<>();
    private int spawnCounter = 0;
    private final Random random = new Random();

    // Game state
    private State state = State.READY;
    private int score = 0;
    private int highScore = 0;
    private Timer gameTimer;

    public FlappyBird() {
        setPreferredSize(new Dimension(SCREEN_WIDTH, SCREEN_HEIGHT));
        setBackground(new Color(78, 192, 202)); // Sky blue
        setFocusable(true);
        addKeyListener(this);
        addMouseListener(this);

        resetGame();

        // 60 FPS Game Loop
        gameTimer = new Timer(16, this);
        gameTimer.start();
    }

    public void resetGame() {
        state = State.READY;
        score = 0;
        birdY = (SCREEN_HEIGHT - GROUND_HEIGHT) / 2.0;
        birdVy = 0.0;
        pipes.clear();
        spawnCounter = 0;
        repaint();
    }

    public void flap() {
        if (state == State.READY) {
            state = State.PLAYING;
            birdVy = FLAP_STRENGTH;
        } else if (state == State.PLAYING) {
            birdVy = FLAP_STRENGTH;
        } else if (state == State.GAMEOVER) {
            resetGame();
        }
    }

    private void die() {
        state = State.GAMEOVER;
        if (score > highScore) {
            highScore = score;
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        updateGame();
        repaint();
    }

    private void updateGame() {
        int floorY = SCREEN_HEIGHT - GROUND_HEIGHT;

        if (state == State.READY) {
            // Idle bobbing
            long now = System.currentTimeMillis();
            birdY = (floorY / 2.0) + Math.sin(now * 0.006) * 8.0;
            return;
        }

        if (state == State.GAMEOVER) {
            if (birdY + BIRD_RADIUS < floorY) {
                birdVy += GRAVITY;
                birdY += birdVy;
            } else {
                birdY = floorY - BIRD_RADIUS;
            }
            return;
        }

        // --- PLAYING STATE ---
        // 1. Gravity & Position
        birdVy += GRAVITY;
        if (birdVy > MAX_FALL_SPEED) birdVy = MAX_FALL_SPEED;
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

        // 2. Pipe Spawning
        spawnCounter++;
        if (spawnCounter >= PIPE_SPAWN_INTERVAL) {
            spawnCounter = 0;
            int gapY = 120 + random.nextInt(320); // 120 to 440
            pipes.add(new Pipe(SCREEN_WIDTH + 10, gapY));
        }

        // 3. Pipes Movement & Collision
        double birdLeft = BIRD_X - BIRD_RADIUS + 4;
        double birdRight = BIRD_X + BIRD_RADIUS - 4;
        double birdTop = birdY - BIRD_RADIUS + 4;
        double birdBottom = birdY + BIRD_RADIUS - 4;

        Iterator<Pipe> it = pipes.iterator();
        while (it.hasNext()) {
            Pipe p = it.next();
            p.x -= PIPE_SPEED;

            // Score trigger
            if (!p.scored && p.x + PIPE_WIDTH < BIRD_X) {
                p.scored = true;
                score++;
            }

            // AABB Collision
            int topPipeBottom = p.gapY - (PIPE_GAP / 2);
            int botPipeTop = p.gapY + (PIPE_GAP / 2);

            if (birdRight > p.x && birdLeft < p.x + PIPE_WIDTH) {
                if (birdTop < topPipeBottom || birdBottom > botPipeTop) {
                    die();
                    return;
                }
            }

            // Remove off-screen
            if (p.x + PIPE_WIDTH < -20) {
                it.remove();
            }
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        int floorY = SCREEN_HEIGHT - GROUND_HEIGHT;

        // 1. Draw Pipes
        Color pipeBody = new Color(115, 191, 46);
        Color pipeBorder = new Color(85, 128, 34);
        int capH = 24;
        int capOverhang = 5;

        for (Pipe p : pipes) {
            int px = (int) p.x;
            int topPipeBottom = p.gapY - (PIPE_GAP / 2);
            int botPipeTop = p.gapY + (PIPE_GAP / 2);

            // Top pipe
            g2.setColor(pipeBody);
            g2.fillRect(px, 0, PIPE_WIDTH, topPipeBottom - capH);
            g2.setColor(pipeBorder);
            g2.drawRect(px, 0, PIPE_WIDTH, topPipeBottom - capH);

            // Top pipe cap
            g2.setColor(pipeBody);
            g2.fillRect(px - capOverhang, topPipeBottom - capH, PIPE_WIDTH + capOverhang * 2, capH);
            g2.setColor(pipeBorder);
            g2.drawRect(px - capOverhang, topPipeBottom - capH, PIPE_WIDTH + capOverhang * 2, capH);

            // Bottom pipe cap
            g2.setColor(pipeBody);
            g2.fillRect(px - capOverhang, botPipeTop, PIPE_WIDTH + capOverhang * 2, capH);
            g2.setColor(pipeBorder);
            g2.drawRect(px - capOverhang, botPipeTop, PIPE_WIDTH + capOverhang * 2, capH);

            // Bottom pipe body
            g2.setColor(pipeBody);
            g2.fillRect(px, botPipeTop + capH, PIPE_WIDTH, floorY - (botPipeTop + capH));
            g2.setColor(pipeBorder);
            g2.drawRect(px, botPipeTop + capH, PIPE_WIDTH, floorY - (botPipeTop + capH));
        }

        // 2. Draw Ground
        g2.setColor(new Color(222, 216, 149));
        g2.fillRect(0, floorY, SCREEN_WIDTH, GROUND_HEIGHT);
        g2.setColor(new Color(115, 191, 46));
        g2.fillRect(0, floorY, SCREEN_WIDTH, 14);
        g2.setColor(new Color(85, 128, 34));
        g2.drawLine(0, floorY, SCREEN_WIDTH, floorY);

        // 3. Draw Bird (with pitch rotation)
        AffineTransform oldTransform = g2.getTransform();
        g2.translate(BIRD_X, (int) birdY);

        double pitchAngle = birdVy > 0
                ? Math.min(Math.toRadians(70), birdVy * 0.08)
                : Math.max(Math.toRadians(-25), birdVy * 0.08);
        g2.rotate(pitchAngle);

        int r = BIRD_RADIUS;
        // Body
        g2.setColor(new Color(248, 231, 28));
        g2.fillOval(-r, -r, r * 2, r * 2);
        g2.setColor(new Color(208, 139, 0));
        g2.drawOval(-r, -r, r * 2, r * 2);

        // Wing
        int wingOff = birdVy < 0 ? -3 : 2;
        g2.setColor(Color.WHITE);
        g2.fillOval(-r + 4, wingOff - 4, 15, 11);
        g2.setColor(new Color(208, 139, 0));
        g2.drawOval(-r + 4, wingOff - 4, 15, 11);

        // Eye
        g2.setColor(Color.WHITE);
        g2.fillOval(3, -11, 11, 11);
        g2.setColor(Color.BLACK);
        g2.fillOval(8, -8, 5, 5);

        // Beak
        g2.setColor(new Color(245, 107, 42));
        g2.fillPolygon(new int[]{10, 20, 10}, new int[]{-3, 1, 6}, 3);

        g2.setTransform(oldTransform);

        // 4. Overlays & HUD
        if (state == State.READY) {
            drawCenteredShadowText(g2, "FLAPPY BIRD", SCREEN_WIDTH / 2, 190, 36, Color.WHITE);
            drawCenteredShadowText(g2, "Press SPACE or CLICK to Flap", SCREEN_WIDTH / 2, 260, 18, new Color(248, 231, 28));
            drawCenteredShadowText(g2, "High Score: " + highScore, SCREEN_WIDTH / 2, 320, 16, Color.WHITE);
        } else if (state == State.PLAYING) {
            drawCenteredShadowText(g2, String.valueOf(score), SCREEN_WIDTH / 2, 70, 42, Color.WHITE);
        } else if (state == State.GAMEOVER) {
            drawCenteredShadowText(g2, "GAME OVER", SCREEN_WIDTH / 2, 180, 38, new Color(231, 76, 60));
            drawCenteredShadowText(g2, "Score: " + score, SCREEN_WIDTH / 2, 250, 26, Color.WHITE);
            drawCenteredShadowText(g2, "Best: " + highScore, SCREEN_WIDTH / 2, 300, 18, new Color(248, 231, 28));
            drawCenteredShadowText(g2, "Press R or SPACE to Restart", SCREEN_WIDTH / 2, 365, 18, Color.WHITE);
        }
    }

    private void drawCenteredShadowText(Graphics2D g2, String text, int x, int y, int fontSize, Color color) {
        g2.setFont(new Font("SansSerif", Font.BOLD, fontSize));
        FontMetrics fm = g2.getFontMetrics();
        int tx = x - (fm.stringWidth(text) / 2);

        // Shadow
        g2.setColor(new Color(40, 40, 40));
        g2.drawString(text, tx + 2, y + 2);

        // Main text
        g2.setColor(color);
        g2.drawString(text, tx, y);
    }

    // Input handlers
    @Override
    public void keyPressed(KeyEvent e) {
        int code = e.getKeyCode();
        if (code == KeyEvent.VK_SPACE || code == KeyEvent.VK_UP || code == KeyEvent.VK_W) {
            flap();
        } else if (code == KeyEvent.VK_R) {
            if (state == State.GAMEOVER) {
                resetGame();
            }
        }
    }

    @Override
    public void mousePressed(MouseEvent e) {
        flap();
    }

    @Override public void keyTyped(KeyEvent e) {}
    @Override public void keyReleased(KeyEvent e) {}
    @Override public void mouseClicked(MouseEvent e) {}
    @Override public void mouseReleased(MouseEvent e) {}
    @Override public void mouseEntered(MouseEvent e) {}
    @Override public void mouseExited(MouseEvent e) {}

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Flappy Bird (Java / Swing)");
            FlappyBird game = new FlappyBird();
            frame.add(game);
            frame.pack();
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setLocationRelativeTo(null);
            frame.setResizable(false);
            frame.setVisible(true);
        });
    }
}
