package tetris;

import javax.microedition.lcdui.*;
import javax.microedition.media.Manager;
import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordStore;
import java.util.Random;

/**
 * Authentic J2ME Tetris Canvas for Nokia E63 (320x240 Landscape Display).
 * Compatible with MIDP 2.0 / CLDC 1.1 on Symbian S60 3rd Edition.
 */
public class TetrisCanvas extends Canvas implements Runnable {

    public static final int SCREEN_W = 320;
    public static final int SCREEN_H = 240;

    public static final int GRID_COLS = 10;
    public static final int GRID_ROWS = 20;
    public static final int CELL_SIZE = 10; // 100px x 200px playfield

    public static final int BOARD_X = 25;
    public static final int BOARD_Y = 20;

    // Tetromino shapes: [piece][rot][block][col/row]
    private static final int[][][][] TETROMINO_SHAPES = new int[][][][] {
        // 0: I (Cyan)
        {
            {{0,1}, {1,1}, {2,1}, {3,1}},
            {{2,0}, {2,1}, {2,2}, {2,3}},
            {{0,2}, {1,2}, {2,2}, {3,2}},
            {{1,0}, {1,1}, {1,2}, {1,3}}
        },
        // 1: O (Yellow)
        {
            {{1,0}, {2,0}, {1,1}, {2,1}},
            {{1,0}, {2,0}, {1,1}, {2,1}},
            {{1,0}, {2,0}, {1,1}, {2,1}},
            {{1,0}, {2,0}, {1,1}, {2,1}}
        },
        // 2: T (Purple)
        {
            {{1,0}, {0,1}, {1,1}, {2,1}},
            {{1,0}, {1,1}, {2,1}, {1,2}},
            {{0,1}, {1,1}, {2,1}, {1,2}},
            {{1,0}, {0,1}, {1,1}, {1,2}}
        },
        // 3: S (Green)
        {
            {{1,0}, {2,0}, {0,1}, {1,1}},
            {{1,0}, {1,1}, {2,1}, {2,2}},
            {{1,1}, {2,1}, {0,2}, {1,2}},
            {{0,0}, {0,1}, {1,1}, {1,2}}
        },
        // 4: Z (Red)
        {
            {{0,0}, {1,0}, {1,1}, {2,1}},
            {{2,0}, {1,1}, {2,1}, {1,2}},
            {{0,1}, {1,1}, {1,2}, {2,2}},
            {{1,0}, {0,1}, {1,1}, {0,2}}
        },
        // 5: J (Blue)
        {
            {{0,0}, {0,1}, {1,1}, {2,1}},
            {{1,0}, {2,0}, {1,1}, {1,2}},
            {{0,1}, {1,1}, {2,1}, {2,2}},
            {{1,0}, {1,1}, {0,2}, {1,2}}
        },
        // 6: L (Orange)
        {
            {{2,0}, {0,1}, {1,1}, {2,1}},
            {{1,0}, {1,1}, {1,2}, {2,2}},
            {{0,1}, {1,1}, {2,1}, {0,2}},
            {{0,0}, {1,0}, {1,1}, {1,2}}
        }
    };

    // J2ME 24-bit RGB colors
    private static final int[] PIECE_COLORS = new int[] {
        0x00e5ff, // Cyan (I)
        0xffea00, // Yellow (O)
        0xd500f9, // Purple (T)
        0x00e676, // Green (S)
        0xff1744, // Red (Z)
        0x2979ff, // Blue (J)
        0xff9100  // Orange (L)
    };

    private final int[][] grid = new int[GRID_ROWS][GRID_COLS];

    private int currentPiece;
    private int currentRot;
    private int currentCol;
    private int currentRow;
    private int nextPiece;

    private int score = 0;
    private int linesCleared = 0;
    private int level = 1;
    private int highScore = 0;
    private boolean gameOver = false;

    private int dropInterval = 25; // frames per step drop
    private int dropTimer = 0;

    private boolean running = false;
    private Thread gameThread;
    private final MIDlet midlet;
    private final Random rand = new Random();

    private static final String RMS_NAME = "TetrisScore";

    public TetrisCanvas(MIDlet midlet) {
        this.midlet = midlet;
        setFullScreenMode(true);
        loadHighScore();
        restartGame();
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

    public void restartGame() {
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                grid[r][c] = 0;
            }
        }
        score = 0;
        linesCleared = 0;
        level = 1;
        gameOver = false;
        dropInterval = 25;
        dropTimer = 0;

        nextPiece = rand.nextInt(7);
        spawnNewPiece();
        repaint();
    }

    private void spawnNewPiece() {
        currentPiece = nextPiece;
        nextPiece = rand.nextInt(7);
        currentRot = 0;
        currentCol = 3;
        currentRow = 0;

        if (!isValidPosition(currentPiece, currentRot, currentCol, currentRow)) {
            gameOver = true;
            playBeep(48, 150); // Low game over tone
            if (score > highScore) {
                highScore = score;
                saveHighScore();
            }
        }
    }

    private boolean isValidPosition(int piece, int rot, int col, int row) {
        int[][] blocks = TETROMINO_SHAPES[piece][rot];
        for (int i = 0; i < 4; i++) {
            int c = col + blocks[i][0];
            int r = row + blocks[i][1];
            if (c < 0 || c >= GRID_COLS || r >= GRID_ROWS) {
                return false;
            }
            if (r >= 0 && grid[r][c] != 0) {
                return false;
            }
        }
        return true;
    }

    public void moveLeft() {
        if (gameOver) return;
        if (isValidPosition(currentPiece, currentRot, currentCol - 1, currentRow)) {
            currentCol--;
            repaint();
        }
    }

    public void moveRight() {
        if (gameOver) return;
        if (isValidPosition(currentPiece, currentRot, currentCol + 1, currentRow)) {
            currentCol++;
            repaint();
        }
    }

    public void rotatePiece() {
        if (gameOver) return;
        int nextRot = (currentRot + 1) % 4;

        if (isValidPosition(currentPiece, nextRot, currentCol, currentRow)) {
            currentRot = nextRot;
            playBeep(76, 25);
        } else if (isValidPosition(currentPiece, nextRot, currentCol - 1, currentRow)) {
            currentCol--;
            currentRot = nextRot;
            playBeep(76, 25);
        } else if (isValidPosition(currentPiece, nextRot, currentCol + 1, currentRow)) {
            currentCol++;
            currentRot = nextRot;
            playBeep(76, 25);
        }
        repaint();
    }

    public void softDrop() {
        if (gameOver) return;
        if (isValidPosition(currentPiece, currentRot, currentCol, currentRow + 1)) {
            currentRow++;
            score += 1;
            dropTimer = 0;
        } else {
            lockPiece();
        }
        repaint();
    }

    public void hardDrop() {
        if (gameOver) return;
        int dist = 0;
        while (isValidPosition(currentPiece, currentRot, currentCol, currentRow + 1)) {
            currentRow++;
            dist++;
        }
        score += dist * 2;
        playBeep(64, 40);
        lockPiece();
        repaint();
    }

    private void lockPiece() {
        int[][] blocks = TETROMINO_SHAPES[currentPiece][currentRot];
        for (int i = 0; i < 4; i++) {
            int c = currentCol + blocks[i][0];
            int r = currentRow + blocks[i][1];
            if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
                grid[r][c] = currentPiece + 1;
            }
        }
        clearLines();
        spawnNewPiece();
    }

    private void clearLines() {
        int cleared = 0;
        for (int r = 0; r < GRID_ROWS; r++) {
            boolean full = true;
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid[r][c] == 0) {
                    full = false;
                    break;
                }
            }
            if (full) {
                cleared++;
                for (int y = r; y > 0; y--) {
                    System.arraycopy(grid[y - 1], 0, grid[y], 0, GRID_COLS);
                }
                for (int c = 0; c < GRID_COLS; c++) {
                    grid[0][c] = 0;
                }
            }
        }

        if (cleared > 0) {
            linesCleared += cleared;
            int[] points = {0, 100, 300, 500, 800};
            score += points[Math.min(cleared, 4)] * level;
            level = 1 + (linesCleared / 10);
            dropInterval = Math.max(3, 25 - (level * 2));
            playBeep(88, 80); // Line clear jingle tone
        }
    }

    private void playBeep(int note, int dur) {
        try {
            Manager.playTone(note, dur, 75);
        } catch (Throwable ignored) {}
    }

    public void run() {
        while (running) {
            long startTime = System.currentTimeMillis();

            if (!gameOver) {
                dropTimer++;
                if (dropTimer >= dropInterval) {
                    dropTimer = 0;
                    if (isValidPosition(currentPiece, currentRot, currentCol, currentRow + 1)) {
                        currentRow++;
                    } else {
                        lockPiece();
                    }
                    repaint();
                }
            }

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

    protected void paint(Graphics g) {
        // 1. Dark Screen Background
        g.setColor(0x11151c);
        g.fillRect(0, 0, SCREEN_W, SCREEN_H);

        // 2. Playfield Background
        int pw = GRID_COLS * CELL_SIZE; // 100
        int ph = GRID_ROWS * CELL_SIZE; // 200

        g.setColor(0x1e2430);
        g.fillRect(BOARD_X, BOARD_Y, pw, ph);
        g.setColor(0x3a4454);
        g.drawRect(BOARD_X - 1, BOARD_Y - 1, pw + 1, ph + 1);

        // Grid lines
        g.setColor(0x252c3b);
        for (int r = 1; r < GRID_ROWS; r++) {
            g.drawLine(BOARD_X, BOARD_Y + r * CELL_SIZE, BOARD_X + pw, BOARD_Y + r * CELL_SIZE);
        }
        for (int c = 1; c < GRID_COLS; c++) {
            g.drawLine(BOARD_X + c * CELL_SIZE, BOARD_Y, BOARD_X + c * CELL_SIZE, BOARD_Y + ph);
        }

        // 3. Draw Locked Blocks
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                int val = grid[r][c];
                if (val > 0) {
                    drawBlock(g, BOARD_X + c * CELL_SIZE, BOARD_Y + r * CELL_SIZE, PIECE_COLORS[val - 1]);
                }
            }
        }

        // 4. Draw Active Piece
        if (!gameOver) {
            int[][] blocks = TETROMINO_SHAPES[currentPiece][currentRot];
            for (int i = 0; i < 4; i++) {
                int c = currentCol + blocks[i][0];
                int r = currentRow + blocks[i][1];
                if (r >= 0) {
                    drawBlock(g, BOARD_X + c * CELL_SIZE, BOARD_Y + r * CELL_SIZE, PIECE_COLORS[currentPiece]);
                }
            }
        }

        // 5. Sidebar (Nokia E63 320px Landscape Layout)
        Font boldFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM);
        Font smallFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL);
        Font headerFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_LARGE);

        int sx = BOARD_X + pw + 18; // ~143
        int sy = BOARD_Y;

        g.setFont(headerFont);
        g.setColor(0xffea00); // Yellow
        g.drawString("TETRIS", sx, sy, Graphics.LEFT | Graphics.TOP);

        // Next Piece Box
        int nxBoxX = sx;
        int nxBoxY = sy + 25;
        g.setFont(smallFont);
        g.setColor(0x8fa1b8);
        g.drawString("NEXT PIECE", nxBoxX, nxBoxY, Graphics.LEFT | Graphics.TOP);

        g.setColor(0x181e29);
        g.fillRect(nxBoxX, nxBoxY + 14, 52, 42);
        g.setColor(0x3a4454);
        g.drawRect(nxBoxX, nxBoxY + 14, 52, 42);

        // Draw Next Piece
        int[][] nblocks = TETROMINO_SHAPES[nextPiece][0];
        for (int i = 0; i < 4; i++) {
            int bc = nblocks[i][0];
            int br = nblocks[i][1];
            drawBlock(g, nxBoxX + 8 + (bc * 8), nxBoxY + 18 + (br * 8), 8, PIECE_COLORS[nextPiece]);
        }

        // Stats Box
        int statsY = nxBoxY + 64;
        g.setFont(smallFont);
        g.setColor(0x8fa1b8);
        g.drawString("SCORE: " + score, sx, statsY, Graphics.LEFT | Graphics.TOP);
        g.drawString("LINES: " + linesCleared, sx, statsY + 15, Graphics.LEFT | Graphics.TOP);
        g.drawString("LEVEL: " + level, sx, statsY + 30, Graphics.LEFT | Graphics.TOP);
        g.drawString("BEST:  " + highScore, sx, statsY + 45, Graphics.LEFT | Graphics.TOP);

        // Controls Reference for Nokia E63
        int ctrlY = statsY + 68;
        g.setColor(0x3a4454);
        g.drawLine(sx, ctrlY, SCREEN_W - 10, ctrlY);
        g.setFont(smallFont);
        g.setColor(0x708090);
        g.drawString("NAVI / A-D : Move", sx, ctrlY + 4, Graphics.LEFT | Graphics.TOP);
        g.drawString("UP / W : Rotate", sx, ctrlY + 16, Graphics.LEFT | Graphics.TOP);
        g.drawString("DOWN / S : Soft Drop", sx, ctrlY + 28, Graphics.LEFT | Graphics.TOP);
        g.drawString("SPACE / ENTER : Hard", sx, ctrlY + 40, Graphics.LEFT | Graphics.TOP);

        // 6. Game Over Overlay
        if (gameOver) {
            g.setColor(0x000000);
            g.fillRect(BOARD_X + 10, BOARD_Y + 70, pw - 20, 50);
            g.setColor(0xe74c3c);
            g.drawRect(BOARD_X + 10, BOARD_Y + 70, pw - 20, 50);

            g.setFont(boldFont);
            g.drawString("GAME OVER", BOARD_X + (pw / 2), BOARD_Y + 82, Graphics.HCENTER | Graphics.TOP);
            g.setFont(smallFont);
            g.setColor(0xffffff);
            g.drawString("Press R / NAVI", BOARD_X + (pw / 2), BOARD_Y + 102, Graphics.HCENTER | Graphics.TOP);
        }
    }

    private void drawBlock(Graphics g, int x, int y, int rgb) {
        drawBlock(g, x, y, CELL_SIZE, rgb);
    }

    private void drawBlock(Graphics g, int x, int y, int size, int rgb) {
        g.setColor(rgb);
        g.fillRect(x + 1, y + 1, size - 2, size - 2);

        // Subtle highlight
        g.setColor(0xffffff);
        g.drawLine(x + 1, y + 1, x + size - 2, y + 1);
        g.drawLine(x + 1, y + 1, x + 1, y + size - 2);

        // Shadow border
        g.setColor(0x000000);
        g.drawLine(x + 1, y + size - 2, x + size - 2, y + size - 2);
        g.drawLine(x + size - 2, y + 1, x + size - 2, y + size - 2);
    }

    protected void keyPressed(int keyCode) {
        int action = getGameAction(keyCode);
        if (action == LEFT || keyCode == 'a' || keyCode == 'A') {
            moveLeft();
        } else if (action == RIGHT || keyCode == 'd' || keyCode == 'D') {
            moveRight();
        } else if (action == UP || keyCode == 'w' || keyCode == 'W') {
            rotatePiece();
        } else if (action == DOWN || keyCode == 's' || keyCode == 'S') {
            softDrop();
        } else if (action == FIRE || keyCode == 32 || keyCode == 10) {
            hardDrop();
        } else if (keyCode == 'r' || keyCode == 'R') {
            restartGame();
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
                highScore = ((b[0] & 0xFF) << 24) | ((b[1] & 0xFF) << 16) | ((b[2] & 0xFF) << 8) | (b[3] & 0xFF);
            }
            rs.closeRecordStore();
        } catch (Throwable ignored) {}
    }

    private void saveHighScore() {
        try {
            RecordStore rs = RecordStore.openRecordStore(RMS_NAME, true);
            byte[] b = new byte[]{
                (byte) ((highScore >> 24) & 0xFF),
                (byte) ((highScore >> 16) & 0xFF),
                (byte) ((highScore >> 8) & 0xFF),
                (byte) (highScore & 0xFF)
            };
            if (rs.getNumRecords() > 0) {
                rs.setRecord(1, b, 0, b.length);
            } else {
                rs.addRecord(b, 0, b.length);
            }
            rs.closeRecordStore();
        } catch (Throwable ignored) {}
    }
}
