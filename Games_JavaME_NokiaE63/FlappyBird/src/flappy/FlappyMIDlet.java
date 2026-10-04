package flappy;

import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * Flappy Bird MIDlet for Nokia E63 (Symbian S60 3rd Edition).
 */
public class FlappyMIDlet extends MIDlet {

    private FlappyCanvas canvas;

    public FlappyMIDlet() {
        super();
    }

    public void startApp() throws MIDletStateChangeException {
        if (canvas == null) {
            canvas = new FlappyCanvas(this);
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
