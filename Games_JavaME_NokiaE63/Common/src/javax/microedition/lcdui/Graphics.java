package javax.microedition.lcdui;

import java.awt.Color;
import java.awt.Graphics2D;

public class Graphics {
    public static final int HCENTER = 1;
    public static final int VCENTER = 2;
    public static final int LEFT = 4;
    public static final int RIGHT = 8;
    public static final int TOP = 16;
    public static final int BOTTOM = 32;
    public static final int BASELINE = 64;

    private Graphics2D g2;
    private Font currentFont;
    private int currentColor = 0x000000;

    public Graphics() {
        this.currentFont = Font.getDefaultFont();
    }

    public void setAwtGraphics(Graphics2D g2) {
        this.g2 = g2;
        if (g2 != null && currentFont != null) {
            g2.setFont(currentFont.getAwtFont());
        }
    }

    public Graphics2D getAwtGraphics() {
        return g2;
    }

    public void setColor(int RGB) {
        this.currentColor = RGB & 0x00FFFFFF;
        if (g2 != null) {
            g2.setColor(new Color(currentColor));
        }
    }

    public void setColor(int red, int green, int blue) {
        setColor(((red & 0xFF) << 16) | ((green & 0xFF) << 8) | (blue & 0xFF));
    }

    public int getColor() {
        return currentColor;
    }

    public void setFont(Font font) {
        this.currentFont = (font != null) ? font : Font.getDefaultFont();
        if (g2 != null) {
            g2.setFont(this.currentFont.getAwtFont());
        }
    }

    public Font getFont() {
        return currentFont;
    }

    public void fillRect(int x, int y, int width, int height) {
        if (g2 != null) g2.fillRect(x, y, width, height);
    }

    public void drawRect(int x, int y, int width, int height) {
        if (g2 != null) g2.drawRect(x, y, width, height);
    }

    public void fillRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        if (g2 != null) g2.fillRoundRect(x, y, width, height, arcWidth, arcHeight);
    }

    public void drawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        if (g2 != null) g2.drawRoundRect(x, y, width, height, arcWidth, arcHeight);
    }

    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        if (g2 != null) g2.fillArc(x, y, width, height, startAngle, arcAngle);
    }

    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        if (g2 != null) g2.drawArc(x, y, width, height, startAngle, arcAngle);
    }

    public void drawLine(int x1, int y1, int x2, int y2) {
        if (g2 != null) g2.drawLine(x1, y1, x2, y2);
    }

    public void drawString(String str, int x, int y, int anchor) {
        if (str == null || g2 == null) return;

        Font font = (currentFont != null) ? currentFont : Font.getDefaultFont();
        int strW = font.stringWidth(str);
        int strH = font.getHeight();
        int baseLine = font.getBaselinePosition();

        int drawX = x;
        if ((anchor & HCENTER) != 0) {
            drawX = x - (strW / 2);
        } else if ((anchor & RIGHT) != 0) {
            drawX = x - strW;
        }

        int drawY = y;
        if ((anchor & TOP) != 0) {
            drawY = y + baseLine;
        } else if ((anchor & BOTTOM) != 0) {
            drawY = y - (strH - baseLine);
        } else if ((anchor & VCENTER) != 0) {
            drawY = y + (baseLine - (strH / 2));
        }

        g2.drawString(str, drawX, drawY);
    }

    public void translate(int x, int y) {
        if (g2 != null) g2.translate(x, y);
    }

    public int getTranslateX() {
        return (g2 != null) ? (int) g2.getTransform().getTranslateX() : 0;
    }

    public int getTranslateY() {
        return (g2 != null) ? (int) g2.getTransform().getTranslateY() : 0;
    }

    public void setClip(int x, int y, int width, int height) {
        if (g2 != null) g2.setClip(x, y, width, height);
    }

    public void clipRect(int x, int y, int width, int height) {
        if (g2 != null) g2.clipRect(x, y, width, height);
    }

    public int getClipX() {
        return (g2 != null && g2.getClipBounds() != null) ? g2.getClipBounds().x : 0;
    }

    public int getClipY() {
        return (g2 != null && g2.getClipBounds() != null) ? g2.getClipBounds().y : 0;
    }

    public int getClipWidth() {
        return (g2 != null && g2.getClipBounds() != null) ? g2.getClipBounds().width : 320;
    }

    public int getClipHeight() {
        return (g2 != null && g2.getClipBounds() != null) ? g2.getClipBounds().height : 240;
    }
}
