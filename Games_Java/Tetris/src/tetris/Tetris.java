package tetris;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.Random;

/**
 * Tetris Implementation in Java (Swing / AWT).
 * Learning implementation comparing Unreal C++, Unity C#, Python, and Java.
 * Self-contained, runnable with standard JDK.
 */
public class Tetris extends JPanel implements ActionListener {

    public static final int GRID_COLS = 10;
    public static final int GRID_ROWS = 20;
    public static final int CELL_SIZE = 28;

    public static final int BOARD_PIXEL_W = GRID_COLS * CELL_SIZE;
    public static final int BOARD_PIXEL_H = GRID_ROWS * CELL_SIZE;
    public static final int SIDEBAR_W = 180;
    public static final int WINDOW_W = BOARD_PIXEL_W + SIDEBAR_W + 40;
    public static final int WINDOW_H = BOARD_PIXEL_H + 40;

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

    private static final Color[] PIECE_COLORS = new Color[] {
        new Color(0, 229, 255),   // Cyan (I)
        new Color(255, 234, 0),   // Yellow (O)
        new Color(213, 0, 249),   // Purple (T)
        new Color(0, 230, 118),   // Green (S)
        new Color(255, 23, 68),   // Red (Z)
        new Color(41, 121, 255),  // Blue (J)
        new Color(255, 145, 0)    // Orange (L)
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
    private boolean isGameOver = false;

    private final double baseDropInterval = 0.8;
    private double currentDropInterval = baseDropInterval;
    private long lastDropTime;

    private final Timer loopTimer;
    private final Random random = new Random();

    private final int boardOriginX = 20;
    private final int boardOriginY = 20;

    public Tetris() {
        setPreferredSize(new Dimension(WINDOW_W, WINDOW_H));
        setBackground(new Color(17, 21, 28));
        setFocusable(true);

        addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                handleKeyPress(e);
            }
        });

        restartGame();

        // ~60 FPS update loop
        loopTimer = new Timer(16, this);
        loopTimer.start();
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
        isGameOver = false;
        currentDropInterval = baseDropInterval;
        lastDropTime = System.currentTimeMillis();

        nextPiece = random.nextInt(7);
        spawnNewPiece();
        repaint();
    }

    private void spawnNewPiece() {
        currentPiece = nextPiece;
        nextPiece = random.nextInt(7);
        currentRot = 0;
        currentCol = 3;
        currentRow = 0;

        if (!isValidPosition(currentPiece, currentRot, currentCol, currentRow)) {
            isGameOver = true;
        }
    }

    private boolean isValidPosition(int piece, int rot, int col, int row) {
        int[][] blocks = TETROMINO_SHAPES[piece][rot];
        for (int[] b : blocks) {
            int c = col + b[0];
            int r = row + b[1];

            if (c < 0 || c >= GRID_COLS || r >= GRID_ROWS) {
                return false;
            }
            if (r >= 0 && grid[r][c] != 0) {
                return false;
            }
        }
        return true;
    }

    private void moveLeft() {
        if (isGameOver) return;
        if (isValidPosition(currentPiece, currentRot, currentCol - 1, currentRow)) {
            currentCol--;
            repaint();
        }
    }

    private void moveRight() {
        if (isGameOver) return;
        if (isValidPosition(currentPiece, currentRot, currentCol + 1, currentRow)) {
            currentCol++;
            repaint();
        }
    }

    private void rotatePiece() {
        if (isGameOver) return;
        int nextRot = (currentRot + 1) % 4;

        if (isValidPosition(currentPiece, nextRot, currentCol, currentRow)) {
            currentRot = nextRot;
        } else if (isValidPosition(currentPiece, nextRot, currentCol - 1, currentRow)) {
            currentCol--;
            currentRot = nextRot;
        } else if (isValidPosition(currentPiece, nextRot, currentCol + 1, currentRow)) {
            currentCol++;
            currentRot = nextRot;
        }
        repaint();
    }

    private void softDrop() {
        if (isGameOver) return;
        if (isValidPosition(currentPiece, currentRot, currentCol, currentRow + 1)) {
            currentRow++;
            score += 1;
            lastDropTime = System.currentTimeMillis();
        } else {
            lockPiece();
        }
        repaint();
    }

    private void hardDrop() {
        if (isGameOver) return;
        int dropDist = 0;
        while (isValidPosition(currentPiece, currentRot, currentCol, currentRow + 1)) {
            currentRow++;
            dropDist++;
        }
        score += dropDist * 2;
        lockPiece();
        repaint();
    }

    private void lockPiece() {
        int[][] blocks = TETROMINO_SHAPES[currentPiece][currentRot];
        for (int[] b : blocks) {
            int c = currentCol + b[0];
            int r = currentRow + b[1];
            if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
                grid[r][c] = currentPiece + 1;
            }
        }
        clearLines();
        spawnNewPiece();
    }

    private void clearLines() {
        int clearedThisTurn = 0;
        for (int r = 0; r < GRID_ROWS; r++) {
            boolean full = true;
            for (int c = 0; c < GRID_COLS; c++) {
                if (grid[r][c] == 0) {
                    full = false;
                    break;
                }
            }
            if (full) {
                clearedThisTurn++;
                for (int y = r; y > 0; y--) {
                    System.arraycopy(grid[y - 1], 0, grid[y], 0, GRID_COLS);
                }
                for (int c = 0; c < GRID_COLS; c++) {
                    grid[0][c] = 0;
                }
            }
        }

        if (clearedThisTurn > 0) {
            linesCleared += clearedThisTurn;
            int[] points = {0, 100, 300, 500, 800};
            score += points[Math.min(clearedThisTurn, 4)] * level;
            level = 1 + (linesCleared / 10);
            currentDropInterval = Math.max(0.08, baseDropInterval * Math.pow(0.85, level - 1));
        }
    }

    private int getGhostRow() {
        int ghostR = currentRow;
        while (isValidPosition(currentPiece, currentRot, currentCol, ghostR + 1)) {
            ghostR++;
        }
        return ghostR;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        long now = System.currentTimeMillis();
        if (!isGameOver) {
            if ((now - lastDropTime) / 1000.0 >= currentDropInterval) {
                lastDropTime = now;
                if (isValidPosition(currentPiece, currentRot, currentCol, currentRow + 1)) {
                    currentRow++;
                } else {
                    lockPiece();
                }
                repaint();
            }
        }
    }

    private void handleKeyPress(KeyEvent e) {
        int code = e.getKeyCode();
        if (code == KeyEvent.VK_LEFT || code == KeyEvent.VK_A) {
            moveLeft();
        } else if (code == KeyEvent.VK_RIGHT || code == KeyEvent.VK_D) {
            moveRight();
        } else if (code == KeyEvent.VK_UP || code == KeyEvent.VK_W) {
            rotatePiece();
        } else if (code == KeyEvent.VK_DOWN || code == KeyEvent.VK_S) {
            softDrop();
        } else if (code == KeyEvent.VK_SPACE || code == KeyEvent.VK_ENTER) {
            hardDrop();
        } else if (code == KeyEvent.VK_R) {
            restartGame();
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        int bx = boardOriginX;
        int by = boardOriginY;

        // 1. Board Background
        g2.setColor(new Color(30, 36, 48));
        g2.fillRect(bx, by, BOARD_PIXEL_W, BOARD_PIXEL_H);
        g2.setColor(new Color(58, 68, 84));
        g2.drawRect(bx, by, BOARD_PIXEL_W, BOARD_PIXEL_H);

        // Subtle grid lines
        g2.setColor(new Color(37, 44, 59));
        for (int r = 1; r < GRID_ROWS; r++) {
            g2.drawLine(bx, by + r * CELL_SIZE, bx + BOARD_PIXEL_W, by + r * CELL_SIZE);
        }
        for (int c = 1; c < GRID_COLS; c++) {
            g2.drawLine(bx + c * CELL_SIZE, by, bx + c * CELL_SIZE, by + BOARD_PIXEL_H);
        }

        // 2. Locked Blocks
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                int val = grid[r][c];
                if (val > 0) {
                    drawBlock(g2, bx + c * CELL_SIZE, by + r * CELL_SIZE, PIECE_COLORS[val - 1]);
                }
            }
        }

        // 3. Ghost Piece
        if (!isGameOver) {
            int ghostR = getGhostRow();
            if (ghostR != currentRow) {
                Color cCol = PIECE_COLORS[currentPiece];
                g2.setColor(new Color(cCol.getRed(), cCol.getGreen(), cCol.getBlue(), 120));
                for (int[] b : TETROMINO_SHAPES[currentPiece][currentRot]) {
                    int c = currentCol + b[0];
                    int r = ghostR + b[1];
                    if (r >= 0) {
                        g2.drawRect(bx + c * CELL_SIZE + 2, by + r * CELL_SIZE + 2, CELL_SIZE - 4, CELL_SIZE - 4);
                    }
                }
            }
        }

        // 4. Active Falling Piece
        if (!isGameOver) {
            for (int[] b : TETROMINO_SHAPES[currentPiece][currentRot]) {
                int c = currentCol + b[0];
                int r = currentRow + b[1];
                if (r >= 0) {
                    drawBlock(g2, bx + c * CELL_SIZE, by + r * CELL_SIZE, PIECE_COLORS[currentPiece]);
                }
            }
        }

        // 5. Sidebar Next Piece Preview
        int sx = bx + BOARD_PIXEL_W + 20;
        int sy = by;

        g2.setFont(new Font("SansSerif", Font.BOLD, 14));
        g2.setColor(new Color(143, 161, 184));
        g2.drawString("NEXT PIECE", sx, sy + 15);

        g2.setColor(new Color(24, 30, 41));
        g2.fillRect(sx, sy + 30, 130, 110);
        g2.setColor(new Color(58, 68, 84));
        g2.drawRect(sx, sy + 30, 130, 110);

        int[][] nextBlocks = TETROMINO_SHAPES[nextPiece][0];
        int minC = 9, maxC = 0, minR = 9, maxR = 0;
        for (int[] b : nextBlocks) {
            if (b[0] < minC) minC = b[0];
            if (b[0] > maxC) maxC = b[0];
            if (b[1] < minR) minR = b[1];
            if (b[1] > maxR) maxR = b[1];
        }
        int piecePixW = (maxC - minC + 1) * CELL_SIZE;
        int piecePixH = (maxR - minR + 1) * CELL_SIZE;
        int offX = sx + (130 - piecePixW) / 2 - (minC * CELL_SIZE);
        int offY = sy + 30 + (110 - piecePixH) / 2 - (minR * CELL_SIZE);

        for (int[] b : nextBlocks) {
            drawBlock(g2, offX + b[0] * CELL_SIZE, offY + b[1] * CELL_SIZE, PIECE_COLORS[nextPiece]);
        }

        // 6. Sidebar Score / Lines / Level
        int infoY = sy + 180;
        g2.setColor(new Color(143, 161, 184));
        g2.drawString("SCORE", sx, infoY);
        g2.setColor(Color.WHITE);
        g2.setFont(new Font("SansSerif", Font.BOLD, 22));
        g2.drawString(String.valueOf(score), sx, infoY + 28);

        infoY += 65;
        g2.setFont(new Font("SansSerif", Font.BOLD, 14));
        g2.setColor(new Color(143, 161, 184));
        g2.drawString("LINES", sx, infoY);
        g2.setColor(Color.WHITE);
        g2.setFont(new Font("SansSerif", Font.BOLD, 22));
        g2.drawString(String.valueOf(linesCleared), sx, infoY + 28);

        infoY += 65;
        g2.setFont(new Font("SansSerif", Font.BOLD, 14));
        g2.setColor(new Color(143, 161, 184));
        g2.drawString("LEVEL", sx, infoY);
        g2.setColor(new Color(255, 215, 0));
        g2.setFont(new Font("SansSerif", Font.BOLD, 22));
        g2.drawString(String.valueOf(level), sx, infoY + 28);

        // 7. Controls Box
        infoY += 75;
        g2.setColor(new Color(24, 30, 41));
        g2.fillRect(sx, infoY, 130, 145);
        g2.setColor(new Color(58, 68, 84));
        g2.drawRect(sx, infoY, 130, 145);

        g2.setColor(new Color(143, 161, 184));
        g2.setFont(new Font("SansSerif", Font.BOLD, 12));
        g2.drawString("CONTROLS", sx + 10, infoY + 20);

        g2.setFont(new Font("SansSerif", Font.PLAIN, 11));
        g2.setColor(new Color(200, 200, 200));
        g2.drawString("A/D : Move", sx + 10, infoY + 42);
        g2.drawString("W : Rotate", sx + 10, infoY + 62);
        g2.drawString("S : Soft Drop", sx + 10, infoY + 82);
        g2.drawString("Space : Hard Drop", sx + 10, infoY + 102);
        g2.drawString("R : Restart", sx + 10, infoY + 122);

        // 8. Game Over Overlay
        if (isGameOver) {
            int midX = bx + BOARD_PIXEL_W / 2;
            int midY = by + BOARD_PIXEL_H / 2;

            g2.setColor(new Color(10, 12, 16, 230));
            g2.fillRect(bx + 15, midY - 50, BOARD_PIXEL_W - 30, 100);
            g2.setColor(new Color(231, 76, 60));
            g2.drawRect(bx + 15, midY - 50, BOARD_PIXEL_W - 30, 100);

            g2.setFont(new Font("SansSerif", Font.BOLD, 24));
            FontMetrics fm = g2.getFontMetrics();
            String goText = "GAME OVER";
            g2.drawString(goText, midX - fm.stringWidth(goText) / 2, midY - 10);

            g2.setFont(new Font("SansSerif", Font.PLAIN, 13));
            g2.setColor(Color.WHITE);
            fm = g2.getFontMetrics();
            String subText = "Press R to Play Again";
            g2.drawString(subText, midX - fm.stringWidth(subText) / 2, midY + 25);
        }
    }

    private void drawBlock(Graphics2D g2, int x, int y, Color baseColor) {
        g2.setColor(baseColor);
        g2.fillRect(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2);

        // Top/left highlight
        g2.setColor(new Color(255, 255, 255, 110));
        g2.drawLine(x + 1, y + 1, x + CELL_SIZE - 2, y + 1);
        g2.drawLine(x + 1, y + 1, x + 1, y + CELL_SIZE - 2);

        // Bottom/right shadow
        g2.setColor(new Color(0, 0, 0, 110));
        g2.drawLine(x + 1, y + CELL_SIZE - 2, x + CELL_SIZE - 2, y + CELL_SIZE - 2);
        g2.drawLine(x + CELL_SIZE - 2, y + 1, x + CELL_SIZE - 2, y + CELL_SIZE - 2);
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Tetris (Java / Swing)");
            Tetris game = new Tetris();
            frame.add(game);
            frame.pack();
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setLocationRelativeTo(null);
            frame.setResizable(false);
            frame.setVisible(true);
        });
    }
}
