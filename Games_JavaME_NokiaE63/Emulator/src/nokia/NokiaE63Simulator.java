package nokia;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.midlet.MIDlet;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;

/**
 * Interactive Nokia E63 Hardware Simulator & Desktop Runner.
 * Accurately reproduces the Nokia E63 landscape form factor:
 * - 320 x 240 16M color landscape LCD display
 * - 5-way Navi-key (D-Pad with central select)
 * - Left and Right Softkeys
 * - Physical QWERTY keyboard routing
 */
public class NokiaE63Simulator extends JPanel {

    public static final int LCD_WIDTH = 320;
    public static final int LCD_HEIGHT = 240;

    // Nokia E63 Body Dimensions
    public static final int PHONE_WIDTH = 460;
    public static final int PHONE_HEIGHT = 580;
    public static final int LCD_X = (PHONE_WIDTH - LCD_WIDTH) / 2; // 70
    public static final int LCD_Y = 55;

    private final MIDlet midlet;
    private final BufferedImage lcdBuffer;
    private final Graphics2D lcdGraphics;

    public NokiaE63Simulator(MIDlet midlet) {
        this.midlet = midlet;
        setPreferredSize(new Dimension(PHONE_WIDTH, PHONE_HEIGHT));
        setBackground(new Color(24, 27, 34)); // Dark background around phone

        lcdBuffer = new BufferedImage(LCD_WIDTH, LCD_HEIGHT, BufferedImage.TYPE_INT_RGB);
        lcdGraphics = lcdBuffer.createGraphics();
        lcdGraphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        setFocusable(true);
        requestFocusInWindow();

        // Forward PC keyboard events to J2ME Canvas
        addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                routeKeyEvent(e, true);
            }

            public void keyReleased(KeyEvent e) {
                routeKeyEvent(e, false);
            }
        });

        // Mouse click on phone buttons
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                handleMouseClick(e.getX(), e.getY(), true);
            }

            public void mouseReleased(MouseEvent e) {
                handleMouseClick(e.getX(), e.getY(), false);
            }
        });

        // Hook up LCDUI display repaint listener
        Display.getDisplay(midlet).setRepaintCallback(new Runnable() {
            public void run() {
                repaint();
            }
        });
    }

    private Canvas getActiveCanvas() {
        Displayable d = Display.getDisplay(midlet).getCurrent();
        return (d instanceof Canvas) ? (Canvas) d : null;
    }

    private void routeKeyEvent(KeyEvent e, boolean pressed) {
        Canvas canvas = getActiveCanvas();
        if (canvas == null) return;

        int j2meKey = 0;
        int code = e.getKeyCode();

        switch (code) {
            case KeyEvent.VK_UP:
            case KeyEvent.VK_W:
                j2meKey = -1; // Nokia Up
                break;
            case KeyEvent.VK_DOWN:
            case KeyEvent.VK_S:
                j2meKey = -2; // Nokia Down
                break;
            case KeyEvent.VK_LEFT:
            case KeyEvent.VK_A:
                j2meKey = -3; // Nokia Left
                break;
            case KeyEvent.VK_RIGHT:
            case KeyEvent.VK_D:
                j2meKey = -4; // Nokia Right
                break;
            case KeyEvent.VK_ENTER:
            case KeyEvent.VK_SPACE:
                j2meKey = -5; // Nokia Center Navi Select
                break;
            case KeyEvent.VK_F1:
                j2meKey = Canvas.NOKIA_KEY_SOFTKEY_LEFT;
                break;
            case KeyEvent.VK_F2:
            case KeyEvent.VK_ESCAPE:
                j2meKey = Canvas.NOKIA_KEY_SOFTKEY_RIGHT;
                break;
            case KeyEvent.VK_R:
                j2meKey = 'r';
                break;
            default:
                char ch = e.getKeyChar();
                if (ch != KeyEvent.CHAR_UNDEFINED) {
                    j2meKey = ch;
                }
                break;
        }

        if (j2meKey != 0) {
            if (pressed) {
                canvas.invokeKeyPressed(j2meKey);
            } else {
                canvas.invokeKeyReleased(j2meKey);
            }
            repaint();
        }
    }

    private void handleMouseClick(int mx, int my, boolean pressed) {
        Canvas canvas = getActiveCanvas();
        if (canvas == null) return;

        // Navi-key center is at X=230, Y=355
        int dpadCenterX = PHONE_WIDTH / 2;
        int dpadCenterY = 355;

        // Center select button (radius 18)
        if (Math.hypot(mx - dpadCenterX, my - dpadCenterY) <= 18) {
            sendKey(canvas, -5, pressed);
            return;
        }

        // Up arrow button
        if (mx >= dpadCenterX - 24 && mx <= dpadCenterX + 24 && my >= dpadCenterY - 42 && my <= dpadCenterY - 20) {
            sendKey(canvas, -1, pressed);
            return;
        }
        // Down arrow button
        if (mx >= dpadCenterX - 24 && mx <= dpadCenterX + 24 && my >= dpadCenterY + 20 && my <= dpadCenterY + 42) {
            sendKey(canvas, -2, pressed);
            return;
        }
        // Left arrow button
        if (mx >= dpadCenterX - 45 && mx <= dpadCenterX - 20 && my >= dpadCenterY - 24 && my <= dpadCenterY + 24) {
            sendKey(canvas, -3, pressed);
            return;
        }
        // Right arrow button
        if (mx >= dpadCenterX + 20 && mx <= dpadCenterX + 45 && my >= dpadCenterY - 24 && my <= dpadCenterY + 24) {
            sendKey(canvas, -4, pressed);
            return;
        }

        // Left Softkey (X=115, Y=330)
        if (mx >= 90 && mx <= 145 && my >= 315 && my <= 345) {
            sendKey(canvas, Canvas.NOKIA_KEY_SOFTKEY_LEFT, pressed);
            return;
        }
        // Right Softkey (X=345, Y=330)
        if (mx >= 315 && mx <= 370 && my >= 315 && my <= 345) {
            sendKey(canvas, Canvas.NOKIA_KEY_SOFTKEY_RIGHT, pressed);
            return;
        }

        // Click directly on LCD triggers primary action (Space / Center Navi)
        if (mx >= LCD_X && mx <= LCD_X + LCD_WIDTH && my >= LCD_Y && my <= LCD_Y + LCD_HEIGHT) {
            sendKey(canvas, -5, pressed);
        }
    }

    private void sendKey(Canvas canvas, int code, boolean pressed) {
        if (pressed) {
            canvas.invokeKeyPressed(code);
        } else {
            canvas.invokeKeyReleased(code);
        }
        repaint();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // 1. Draw Phone Body (Nokia E63 Ultramarine Blue / Metallic Charcoal chassis)
        g2.setColor(new Color(36, 44, 58)); // Outer phone chassis
        g2.fillRoundRect(30, 10, PHONE_WIDTH - 60, PHONE_HEIGHT - 20, 48, 48);

        // Metallic bezel border
        g2.setColor(new Color(75, 88, 110));
        g2.setStroke(new BasicStroke(3));
        g2.drawRoundRect(30, 10, PHONE_WIDTH - 60, PHONE_HEIGHT - 20, 48, 48);

        // 2. Top Speaker Earpiece & Camera
        g2.setColor(new Color(20, 24, 32));
        g2.fillRoundRect((PHONE_WIDTH - 65) / 2, 24, 65, 8, 4, 4);

        // NOKIA Logo
        g2.setFont(new Font("SansSerif", Font.BOLD, 13));
        g2.setColor(new Color(190, 205, 225));
        FontMetrics fm = g2.getFontMetrics();
        g2.drawString("NOKIA", (PHONE_WIDTH - fm.stringWidth("NOKIA")) / 2, 46);

        // Model Tag
        g2.setFont(new Font("SansSerif", Font.PLAIN, 10));
        g2.setColor(new Color(130, 145, 165));
        g2.drawString("E63", PHONE_WIDTH - 85, 46);

        // 3. Render 320x240 LCD Screen
        Canvas canvas = getActiveCanvas();
        if (canvas != null) {
            canvas.invokePaint(lcdGraphics);
        } else {
            lcdGraphics.setColor(Color.BLACK);
            lcdGraphics.fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT);
        }

        // Screen bezel frame
        g2.setColor(new Color(12, 15, 20));
        g2.fillRect(LCD_X - 5, LCD_Y - 5, LCD_WIDTH + 10, LCD_HEIGHT + 10);
        g2.setColor(new Color(90, 105, 125));
        g2.drawRect(LCD_X - 6, LCD_Y - 6, LCD_WIDTH + 11, LCD_HEIGHT + 11);

        // Draw LCD Content
        g2.drawImage(lcdBuffer, LCD_X, LCD_Y, null);

        // 4. Navi-Key & Softkeys Area
        int dpadCenterX = PHONE_WIDTH / 2;
        int dpadCenterY = 355;

        // Outer D-Pad Ring
        g2.setColor(new Color(25, 30, 40));
        g2.fillOval(dpadCenterX - 45, dpadCenterY - 45, 90, 90);
        g2.setColor(new Color(130, 145, 170));
        g2.setStroke(new BasicStroke(2));
        g2.drawOval(dpadCenterX - 45, dpadCenterY - 45, 90, 90);

        // Direction Arrows on D-Pad
        g2.setColor(new Color(175, 190, 210));
        drawArrow(g2, dpadCenterX, dpadCenterY - 32, 0);   // Up
        drawArrow(g2, dpadCenterX, dpadCenterY + 32, 180); // Down
        drawArrow(g2, dpadCenterX - 32, dpadCenterY, 270); // Left
        drawArrow(g2, dpadCenterX + 32, dpadCenterY, 90);  // Right

        // Center Navi Button
        g2.setColor(new Color(50, 60, 78));
        g2.fillOval(dpadCenterX - 18, dpadCenterY - 18, 36, 36);
        g2.setColor(new Color(185, 200, 225));
        g2.drawOval(dpadCenterX - 18, dpadCenterY - 18, 36, 36);

        // Left Softkey Button
        g2.setColor(new Color(45, 55, 72));
        g2.fillRoundRect(80, 320, 65, 24, 8, 8);
        g2.setColor(new Color(150, 165, 185));
        g2.setFont(new Font("SansSerif", Font.BOLD, 10));
        g2.drawString("Options", 92, 336);

        // Right Softkey Button
        g2.setColor(new Color(45, 55, 72));
        g2.fillRoundRect(PHONE_WIDTH - 145, 320, 65, 24, 8, 8);
        g2.setColor(new Color(150, 165, 185));
        g2.drawString("Exit", PHONE_WIDTH - 124, 336);

        // Call & End Buttons
        g2.setColor(new Color(35, 120, 45)); // Green Call
        g2.fillRoundRect(80, 355, 45, 22, 6, 6);
        g2.setColor(new Color(160, 35, 35)); // Red End
        g2.fillRoundRect(PHONE_WIDTH - 125, 355, 45, 22, 6, 6);

        // 5. Nokia E63 QWERTY Keyboard Mockup
        int kbY = 415;
        g2.setColor(new Color(28, 34, 46));
        g2.fillRoundRect(50, kbY, PHONE_WIDTH - 100, 140, 12, 12);
        g2.setColor(new Color(60, 72, 92));
        g2.drawRoundRect(50, kbY, PHONE_WIDTH - 100, 140, 12, 12);

        String[] rows = {
            "Q W E R T Y U I O P",
            "A S D F G H J K L",
            "Z X C V B N M  <--"
        };

        g2.setFont(new Font("Monospaced", Font.BOLD, 11));
        g2.setColor(new Color(180, 195, 215));

        for (int r = 0; r < rows.length; r++) {
            String[] keys = rows[r].split(" ");
            int startX = 65 + (r * 12);
            int y = kbY + 28 + (r * 28);
            for (String k : keys) {
                g2.drawString(k, startX, y);
                startX += 28;
            }
        }

        // Spacebar
        g2.setColor(new Color(45, 55, 72));
        g2.fillRoundRect((PHONE_WIDTH - 140) / 2, kbY + 98, 140, 20, 6, 6);
        g2.setColor(new Color(140, 155, 180));
        g2.setFont(new Font("SansSerif", Font.PLAIN, 10));
        g2.drawString("SPACE / JUMP", (PHONE_WIDTH - 80) / 2, kbY + 112);
    }

    private void drawArrow(Graphics2D g2, int cx, int cy, int angleDeg) {
        int[] x = {-4, 0, 4};
        int[] y = {3, -4, 3};
        Polygon p = new Polygon(x, y, 3);

        Graphics2D gCopy = (Graphics2D) g2.create();
        gCopy.translate(cx, cy);
        gCopy.rotate(Math.toRadians(angleDeg));
        gCopy.fill(p);
        gCopy.dispose();
    }

    public static void launch(final MIDlet midlet, final String title) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                JFrame frame = new JFrame(title);
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                NokiaE63Simulator sim = new NokiaE63Simulator(midlet);
                frame.add(sim);
                frame.pack();
                frame.setResizable(false);
                frame.setLocationRelativeTo(null);
                frame.setVisible(true);

                try {
                    midlet.startApp();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public static void main(String[] args) {
        String target = (args.length > 0) ? args[0].toLowerCase() : "flappy";
        if (target.contains("tetris")) {
            launch(new tetris.TetrisMIDlet(), "Nokia E63 - Tetris J2ME");
        } else {
            launch(new flappy.FlappyMIDlet(), "Nokia E63 - Flappy Bird J2ME");
        }
    }
}
