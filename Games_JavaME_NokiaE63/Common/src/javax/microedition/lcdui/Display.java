package javax.microedition.lcdui;

import javax.microedition.midlet.MIDlet;
import java.util.Hashtable;

public class Display {
    private static final Hashtable displays = new Hashtable();
    private Displayable currentDisplayable;
    private Runnable repaintCallback;

    private Display() {}

    public static synchronized Display getDisplay(MIDlet m) {
        if (m == null) return null;
        Display d = (Display) displays.get(m);
        if (d == null) {
            d = new Display();
            displays.put(m, d);
        }
        return d;
    }

    public void setCurrent(Displayable next) {
        this.currentDisplayable = next;
        if (next instanceof Canvas) {
            ((Canvas) next).setHostDisplay(this);
        }
        requestRepaint();
    }

    public Displayable getCurrent() {
        return currentDisplayable;
    }

    public void setRepaintCallback(Runnable r) {
        this.repaintCallback = r;
    }

    public void requestRepaint() {
        if (repaintCallback != null) {
            repaintCallback.run();
        }
    }
}
