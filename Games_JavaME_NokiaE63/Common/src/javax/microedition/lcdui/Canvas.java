package javax.microedition.lcdui;

public abstract class Canvas extends Displayable {
    public static final int UP = 1;
    public static final int DOWN = 6;
    public static final int LEFT = 2;
    public static final int RIGHT = 5;
    public static final int FIRE = 8;
    public static final int GAME_A = 9;
    public static final int GAME_B = 10;
    public static final int GAME_C = 11;
    public static final int GAME_D = 12;

    public static final int KEY_NUM0 = 48;
    public static final int KEY_NUM1 = 49;
    public static final int KEY_NUM2 = 50;
    public static final int KEY_NUM3 = 51;
    public static final int KEY_NUM4 = 52;
    public static final int KEY_NUM5 = 53;
    public static final int KEY_NUM6 = 54;
    public static final int KEY_NUM7 = 55;
    public static final int KEY_NUM8 = 56;
    public static final int KEY_NUM9 = 57;
    public static final int KEY_STAR = 42;
    public static final int KEY_POUND = 35;

    // Nokia specific key codes
    public static final int NOKIA_KEY_SOFTKEY_LEFT = -6;
    public static final int NOKIA_KEY_SOFTKEY_RIGHT = -7;

    private boolean fullScreen = false;
    private final Graphics internalGraphics = new Graphics();
    private Display hostDisplay;

    protected Canvas() {}

    public void setHostDisplay(Display d) {
        this.hostDisplay = d;
    }

    public void setFullScreenMode(boolean mode) {
        this.fullScreen = mode;
    }

    public boolean hasPointerEvents() {
        return false; // Nokia E63 is non-touch
    }

    public boolean hasPointerMotionEvents() {
        return false;
    }

    public boolean hasRepeatEvents() {
        return true;
    }

    public int getGameAction(int keyCode) {
        // Map Nokia E63 Navi-key D-pad and standard keys
        switch (keyCode) {
            case -1: // Nokia Up
            case 'w':
            case 'W':
            case KEY_NUM2:
                return UP;
            case -2: // Nokia Down
            case 's':
            case 'S':
            case KEY_NUM8:
                return DOWN;
            case -3: // Nokia Left
            case 'a':
            case 'A':
            case KEY_NUM4:
                return LEFT;
            case -4: // Nokia Right
            case 'd':
            case 'D':
            case KEY_NUM6:
                return RIGHT;
            case -5: // Nokia Center Navi select
            case 10: // Enter
            case 32: // Space
            case KEY_NUM5:
                return FIRE;
            default:
                return 0;
        }
    }

    public int getKeyCode(int gameAction) {
        switch (gameAction) {
            case UP: return -1;
            case DOWN: return -2;
            case LEFT: return -3;
            case RIGHT: return -4;
            case FIRE: return -5;
            default: return 0;
        }
    }

    public void repaint() {
        if (hostDisplay != null) {
            hostDisplay.requestRepaint();
        }
    }

    public void repaint(int x, int y, int width, int height) {
        repaint();
    }

    public void serviceRepaints() {
        // Synchronous repaint request
        repaint();
    }

    public void invokePaint(java.awt.Graphics2D g2) {
        internalGraphics.setAwtGraphics(g2);
        paint(internalGraphics);
    }

    protected abstract void paint(Graphics g);

    protected void keyPressed(int keyCode) {}
    protected void keyReleased(int keyCode) {}
    protected void keyRepeated(int keyCode) {
        keyPressed(keyCode);
    }

    public void invokeKeyPressed(int keyCode) {
        keyPressed(keyCode);
    }

    public void invokeKeyReleased(int keyCode) {
        keyReleased(keyCode);
    }
}
