@echo off
setlocal

:: Find Java
where java >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set "JAVA_CMD=java"
    set "JAVAC_CMD=javac"
) else if exist "C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\java.exe" (
    set "JAVA_CMD=C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\java.exe"
    set "JAVAC_CMD=C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\javac.exe"
) else (
    echo [ERROR] No Java runtime found in PATH or Unity Hub OpenJDK.
    pause
    exit /b 1
)

echo Compiling FlappyBird.java...
if not exist "bin" mkdir bin
"%JAVAC_CMD%" -d bin src\flappy\FlappyBird.java
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Running Flappy Bird...
"%JAVA_CMD%" -cp bin flappy.FlappyBird
