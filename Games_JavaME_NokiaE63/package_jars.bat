@echo off
setlocal

:: Find jar tool
set "JAR_CMD="
if exist "C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\jar.exe" (
    set "JAR_CMD=C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\jar.exe"
) else (
    where jar >nul 2>nul
    if %ERRORLEVEL% equ 0 set "JAR_CMD=jar"
)

if "%JAR_CMD%"=="" (
    echo [ERROR] jar.exe not found!
    pause
    exit /b 1
)

echo =======================================================
echo Packaging Nokia E63 J2ME MIDP 2.0 / CLDC 1.1 JARs...
echo =======================================================

:: 1. Package Flappy Bird JAR
echo Packaging FlappyBird.jar...
if not exist "dist" mkdir dist
pushd FlappyBird\bin
"%JAR_CMD%" cvfm ..\..\dist\FlappyBird.jar ..\MANIFEST.MF flappy\*.class
popd
copy /y FlappyBird\FlappyBird.jad dist\FlappyBird.jad >nul

:: Update MIDlet-Jar-Size in JAD
for %%I in ("dist\FlappyBird.jar") do set "FLAPPY_SIZE=%%~zI"
echo MIDlet-Jar-Size: %FLAPPY_SIZE% >> dist\FlappyBird.jad
echo MIDlet-Jar-URL: FlappyBird.jar >> dist\FlappyBird.jad
echo [OK] dist\FlappyBird.jar (%FLAPPY_SIZE% bytes) and dist\FlappyBird.jad ready!

:: 2. Package Tetris JAR
echo.
echo Packaging Tetris.jar...
pushd Tetris\bin
"%JAR_CMD%" cvfm ..\..\dist\Tetris.jar ..\MANIFEST.MF tetris\*.class
popd
copy /y Tetris\Tetris.jad dist\Tetris.jad >nul

for %%I in ("dist\Tetris.jar") do set "TETRIS_SIZE=%%~zI"
echo MIDlet-Jar-Size: %TETRIS_SIZE% >> dist\Tetris.jad
echo MIDlet-Jar-URL: Tetris.jar >> dist\Tetris.jad
echo [OK] dist\Tetris.jar (%TETRIS_SIZE% bytes) and dist\Tetris.jad ready!

echo.
echo =======================================================
echo Build complete! Transfer dist\*.jar and dist\*.jad to
echo your Nokia E63 via Bluetooth, USB Mass Storage, or microSD!
echo =======================================================
