package javax.microedition.lcdui;

public class Font {
    public static final int STYLE_PLAIN = 0;
    public static final int STYLE_BOLD = 1;
    public static final int STYLE_ITALIC = 2;
    public static final int STYLE_UNDERLINED = 4;

    public static final int SIZE_SMALL = 8;
    public static final int SIZE_MEDIUM = 0;
    public static final int SIZE_LARGE = 16;

    public static final int FACE_SYSTEM = 0;
    public static final int FACE_MONOSPACE = 32;
    public static final int FACE_PROPORTIONAL = 64;

    private final java.awt.Font awtFont;

    private Font(int face, int style, int size) {
        int awtStyle = java.awt.Font.PLAIN;
        if ((style & STYLE_BOLD) != 0) awtStyle |= java.awt.Font.BOLD;
        if ((style & STYLE_ITALIC) != 0) awtStyle |= java.awt.Font.ITALIC;

        int awtSize = 13;
        if (size == SIZE_SMALL) awtSize = 11;
        else if (size == SIZE_LARGE) awtSize = 16;

        String name = "SansSerif";
        if (face == FACE_MONOSPACE) name = "Monospaced";

        this.awtFont = new java.awt.Font(name, awtStyle, awtSize);
    }

    public static Font getFont(int face, int style, int size) {
        return new Font(face, style, size);
    }

    public static Font getDefaultFont() {
        return new Font(FACE_SYSTEM, STYLE_PLAIN, SIZE_MEDIUM);
    }

    public int stringWidth(String str) {
        if (str == null) return 0;
        java.awt.FontMetrics fm = java.awt.Toolkit.getDefaultToolkit().getFontMetrics(awtFont);
        return fm.stringWidth(str);
    }

    public int getHeight() {
        java.awt.FontMetrics fm = java.awt.Toolkit.getDefaultToolkit().getFontMetrics(awtFont);
        return fm.getHeight();
    }

    public int getBaselinePosition() {
        java.awt.FontMetrics fm = java.awt.Toolkit.getDefaultToolkit().getFontMetrics(awtFont);
        return fm.getAscent();
    }

    public java.awt.Font getAwtFont() {
        return awtFont;
    }
}
