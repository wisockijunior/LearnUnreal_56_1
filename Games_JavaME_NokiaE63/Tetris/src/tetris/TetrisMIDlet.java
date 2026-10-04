package tetris;

import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * Tetris MIDlet for Nokia E63 (Symbian S60 3rd Edition).
 */
public class TetrisMIDlet extends MIDlet {

    private TetrisCanvas canvas;

    public TetrisMIDlet() {
        super();
    }

    public void startApp() throws MIDletStateChangeException {
        if (canvas == null) {
            canvas = new TetrisCanvas(this);
        }
        Display.getDisplay(this).setCurrent(canvas);
        canvas.start();
    }

    public void pauseApp() {
        if (canvas != null) {
            canvas.stop();
        }
    }

    public void destroyApp(boolean unconditional) throws MIDletStateChangeException {
        if (canvas != null) {
            canvas.stop();
        }
    }
}
