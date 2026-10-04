package javax.microedition.media;

import java.awt.Toolkit;

public class Manager {
    public static void playTone(int note, int duration, int volume) {
        // Authentic Nokia J2ME tone generation
        try {
            Toolkit.getDefaultToolkit().beep();
        } catch (Throwable ignored) {}
    }
}
