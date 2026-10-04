package javax.microedition.lcdui;

import java.util.Vector;

public abstract class Displayable {
    protected final Vector commands = new Vector();
    protected CommandListener listener;
    protected int width = 320;  // Nokia E63 default landscape width
    protected int height = 240; // Nokia E63 default landscape height

    public void addCommand(Command cmd) {
        if (cmd != null && !commands.contains(cmd)) {
            commands.addElement(cmd);
        }
    }

    public void removeCommand(Command cmd) {
        if (cmd != null) {
            commands.removeElement(cmd);
        }
    }

    public void setCommandListener(CommandListener l) {
        this.listener = l;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public void setSize(int w, int h) {
        this.width = w;
        this.height = h;
    }

    public boolean isShown() {
        return true;
    }
}
