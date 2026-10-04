package javax.microedition.midlet;

/**
 * Standard J2ME MIDP 2.0 MIDlet base class for Nokia E63.
 */
public abstract class MIDlet {
    protected MIDlet() {}

    public abstract void startApp() throws MIDletStateChangeException;
    public abstract void pauseApp();
    public abstract void destroyApp(boolean unconditional) throws MIDletStateChangeException;

    public void notifyDestroyed() {
        // Handled by runtime environment
        System.exit(0);
    }
}
