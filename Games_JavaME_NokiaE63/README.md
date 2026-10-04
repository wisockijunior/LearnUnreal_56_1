# Nokia E63 J2ME Retro Implementations (Flappy Bird & Tetris)

This directory contains authentic **Java ME (J2ME) MIDP 2.0 / CLDC 1.1** implementations of **Flappy Bird** and **Tetris**, specifically engineered for the classic **Nokia E63** smartphone.

---

## 1. Nokia E63 Hardware & Platform Specifications

| Specification | Nokia E63 Details |
| :--- | :--- |
| **Release Year** | Late 2008 |
| **Operating System** | Symbian OS v9.2, Series 60 (S60) 3rd Edition, Feature Pack 1 |
| **CPU** | ARM926EJ-S running at 369 MHz |
| **Display** | **320 x 240 pixels** (~169 ppi), **Landscape** orientation (2.36 inches, 16M colors) |
| **Keyboard** | Full physical QWERTY keyboard + 5-way Navi-Key (Up, Down, Left, Right, Center Fire) |
| **Java Platform** | **J2ME MIDP 2.0 (JSR-118) / CLDC 1.1 (JSR-139)** |
| **Audio API** | Mobile Media API (MMAPI - JSR-135) & `Manager.playTone()` |
| **Storage API** | Record Management System (RMS - `javax.microedition.rms.RecordStore`) |

---

## 2. Folder Structure

```
Games_JavaME_NokiaE63/
├── Common/                          # Clean J2ME MIDP 2.0 / CLDC 1.1 API definitions
│   └── src/javax/microedition/
│       ├── midlet/MIDlet.java       # Base lifecycle class (startApp, pauseApp, destroyApp)
│       ├── lcdui/Canvas.java        # 320x240 landscape graphics, key mapping for Navi-key & QWERTY
│       ├── lcdui/Graphics.java      # J2ME drawing primitives with LCDUI anchors
│       ├── media/Manager.java       # Hardware retro sound tone generator
│       └── rms/RecordStore.java     # Symbian persistent flash memory record store
│
├── FlappyBird/
│   ├── src/flappy/
│   │   ├── FlappyMIDlet.java        # Entry point MIDlet
│   │   └── FlappyCanvas.java        # 320x240 landscape physics, pipes, RMS score & sound
│   ├── FlappyBird.jad               # Application descriptor
│   └── MANIFEST.MF                  # JAR manifest with Nokia display size metadata
│
├── Tetris/
│   ├── src/tetris/
│   │   ├── TetrisMIDlet.java        # Entry point MIDlet
│   │   └── TetrisCanvas.java        # 10x20 matrix, next piece preview, RMS score, tone beeps
│   ├── Tetris.jad                   # Application descriptor
│   └── MANIFEST.MF
│
├── Emulator/
│   └── src/nokia/
│       └── NokiaE63Simulator.java   # Interactive Nokia E63 hardware skin & desktop runner
│
├── dist/                            # Ready-to-install packages for physical Nokia E63
│   ├── FlappyBird.jar & .jad        # (~6 KB)
│   └── Tetris.jar & .jad            # (~6.5 KB)
│
├── build_all.bat                    # Recompiles all code & packages JARs
├── package_jars.bat                 # Re-packages JARs and updates JAD byte-sizes
├── run_nokia_e63_flappy.bat         # Launches Flappy Bird in the Nokia E63 Simulator
├── run_nokia_e63_tetris.bat         # Launches Tetris in the Nokia E63 Simulator
└── README.md
```

---

## 3. How to Play on PC (Interactive Nokia E63 Simulator)

You don't need a vintage Nokia SDK or Sun Wireless Toolkit. The built-in simulator accurately renders the **Nokia E63 chassis, 320x240 LCD display, 5-way Navi-Key, and QWERTY keypad**:

### Launch Flappy Bird:
Double-click `run_nokia_e63_flappy.bat` or run:
```powershell
.\Games_JavaME_NokiaE63\run_nokia_e63_flappy.bat
```

### Launch Tetris:
Double-click `run_nokia_e63_tetris.bat` or run:
```powershell
.\Games_JavaME_NokiaE63\run_nokia_e63_tetris.bat
```

### Controls (Physical Keyboard or Mouse on Phone Skin):
- **Flappy Bird**:
  - `Space`, `Enter`, `W`, or `Up Arrow`: Flap
  - `R`: Restart
  - *Or click the central Navi-Key button or screen with your mouse!*
- **Tetris**:
  - `Left` / `Right` or `A` / `D`: Move Left / Right
  - `Up` or `W`: Rotate Piece
  - `Down` or `S`: Soft Drop
  - `Space` or `Enter`: Hard Drop
  - `R`: Restart

---

## 4. How to Install on a Physical Nokia E63 Phone

The compiled binaries in [`dist/`](file:///c:/Unreal%20Projects/LearnUnreal_56_1/Games_JavaME_NokiaE63/dist/) are 100% standard J2ME MIDlets:

### Method 1: Bluetooth (Fastest)
1. Turn on Bluetooth on your PC and your Nokia E63.
2. Pair the devices.
3. Right-click `dist\FlappyBird.jar` (or `Tetris.jar`) on Windows $\rightarrow$ **Send to** $\rightarrow$ **Bluetooth device** $\rightarrow$ select your **Nokia E63**.
4. On your Nokia E63, you will receive a message in your **Messaging Inbox**.
5. Open the message and select **Install** $\rightarrow$ choose phone memory or Memory card $\rightarrow$ launch from **Applications / Installations**!

### Method 2: USB Cable or MicroSD Card
1. Connect your Nokia E63 to your PC using a standard micro-USB cable and choose **Mass Storage** on the phone, OR insert the phone's microSD card into your PC.
2. Copy `FlappyBird.jar`, `FlappyBird.jad`, `Tetris.jar`, and `Tetris.jad` to any folder on the card (e.g. `E:\Others\` or `E:\Data\`).
3. On the phone, open **Menu** $\rightarrow$ **Tools** $\rightarrow$ **File Manager** $\rightarrow$ Navigate to where you copied the `.jar`.
4. Press the **Center Navi-Key** on `FlappyBird.jar` or `Tetris.jar` to install.

---

## 5. Architectural Differences: J2ME vs Desktop Java (Swing)

1. **Screen Layout (320 x 240 Landscape)**:
   - Modern mobile screens are tall ($Y$-oriented portrait). The Nokia E63 is one of the iconic **wide landscape** phones ($320\text{px wide} \times 240\text{px high}$).
   - Tetris is partitioned with a $100\text{px} \times 200\text{px}$ matrix on the left, leaving the remaining $200\text{px}$ on the right for next-piece preview, stats, and control labels.

2. **Game Loop (`Thread` vs `javax.swing.Timer`)**:
   - J2ME devices lack Swing/AWT timer architectures. The standard design is a dedicated `Thread` running `while (running) { update(); repaint(); serviceRepaints(); Thread.sleep(30); }`.
   - Targeting ~33 FPS is optimal for the Symbian ARM9 369MHz CPU to balance smoothness and battery life.

3. **Data Persistence (`RecordStore` RMS)**:
   - On Symbian, apps do not have arbitrary file access without signed security certificates. High scores are stored using the **Record Management System (RMS)** (`RecordStore.openRecordStore("FlappyScore", true)`).
   - High scores persist across phone reboots and app launches in flash memory.

4. **Audio (`Manager.playTone`)**:
   - Instead of heavy audio decoding, retro Symbian games utilize hardware FM/synthesizer beeps via `Manager.playTone(note, duration, volume)`.
