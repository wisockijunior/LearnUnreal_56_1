@echo off
setlocal

:: Find Java
set "JAVA_CMD="
if exist "C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\java.exe" (
    set "JAVA_CMD=C:\Program Files\Unity\Hub\Editor\2022.3.25f1\Editor\Data\PlaybackEngines\AndroidPlayer\OpenJDK\bin\java.exe"
) else (
    where java >nul 2>nul
    if %ERRORLEVEL% equ 0 set "JAVA_CMD=java"
)

if "%JAVA_CMD%"=="" (
    echo [ERROR] java.exe not found!
    pause
    exit /b 1
)

echo Starting Nokia E63 Simulator running Flappy Bird (J2ME)...
echo Controls on PC:
echo   - SPACE / UP / W / ENTER: Flap / Jump
echo   - R: Restart
echo   - Click on the D-Pad or screen with the mouse
echo.

"%JAVA_CMD%" -cp "Common\bin;FlappyBird\bin;Emulator\bin" nokia.NokiaE63Simulator flappy
