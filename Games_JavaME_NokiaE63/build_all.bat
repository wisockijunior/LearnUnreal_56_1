@echo off
setlocal

:: Find javac and jar
set "JAVAC_CMD="
set "JAR_CMD="
if exist "C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\javac.exe" (
    set "JAVAC_CMD=C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\javac.exe"
    set "JAR_CMD=C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\jar.exe"
) else (
    where javac >nul 2>nul
    if %ERRORLEVEL% equ 0 set "JAVAC_CMD=javac"
    where jar >nul 2>nul
    if %ERRORLEVEL% equ 0 set "JAR_CMD=jar"
)

if "%JAVAC_CMD%"=="" (
    echo [ERROR] javac.exe not found!
    pause
    exit /b 1
)

echo ========================================================
echo Compiling Nokia E63 J2ME Codebase...
echo ========================================================

:: 1. Compile Common J2ME API Stubs
echo [1/4] Compiling Common J2ME Stubs...
if not exist "Common\bin" mkdir Common\bin
"%JAVAC_CMD%" -d Common\bin Common\src\javax\microedition\midlet\*.java Common\src\javax\microedition\lcdui\*.java Common\src\javax\microedition\media\*.java Common\src\javax\microedition\rms\*.java
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: 2. Compile Flappy Bird MIDlet
echo [2/4] Compiling Flappy Bird MIDlet...
if not exist "FlappyBird\bin" mkdir FlappyBird\bin
"%JAVAC_CMD%" -cp Common\bin -d FlappyBird\bin FlappyBird\src\flappy\*.java
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: 3. Compile Tetris MIDlet
echo [3/4] Compiling Tetris MIDlet...
if not exist "Tetris\bin" mkdir Tetris\bin
"%JAVAC_CMD%" -cp Common\bin -d Tetris\bin Tetris\src\tetris\*.java
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: 4. Compile Simulator
echo [4/4] Compiling Nokia E63 Hardware Simulator...
if not exist "Emulator\bin" mkdir Emulator\bin
"%JAVAC_CMD%" -cp "Common\bin;FlappyBird\bin;Tetris\bin" -d Emulator\bin Emulator\src\nokia\*.java
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: 5. Package JARs
call package_jars.bat

echo.
echo ========================================================
echo ALL BUILD STEPS SUCCESSFUL!
echo To play on PC:
echo   - run_nokia_e63_flappy.bat
echo   - run_nokia_e63_tetris.bat
echo ========================================================
