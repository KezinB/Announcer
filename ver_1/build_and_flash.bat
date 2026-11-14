@echo off
setlocal

set "IDF_PATH=C:\esp\v6.0.2\esp-idf"
set "IDF_TOOLS_PATH=C:\Espressif\tools"
set "IDF_PYTHON_ENV_PATH=C:\Espressif\tools\python\v6.0.2\venv"
set "ESP_IDF_VERSION=6.0"

set "PATH=C:\Espressif\tools\ccache\4.12.1\ccache-4.12.1-windows-x86_64;C:\Espressif\tools\cmake\4.0.3\bin;C:\Espressif\tools\ninja\1.12.1;C:\Espressif\tools\idf-exe\1.0.3;C:\Espressif\tools\xtensa-esp-elf\esp-15.2.0_20251204\xtensa-esp-elf\bin;C:\Espressif\tools\python\v6.0.2\venv\Scripts;%PATH%"

set "PYTHON_EXE=C:\Espressif\tools\python\v6.0.2\venv\Scripts\python.exe"
set "IDF_PY=%IDF_PATH%\tools\idf.py"

echo ============================================================
echo   ESP32-S3 Speaker Test Build & Flash
echo ============================================================

if not exist "%~dp0build" (
    echo [1/3] Setting target to esp32s3...
    "%PYTHON_EXE%" "%IDF_PY%" set-target esp32s3
    if errorlevel 1 goto error
)

echo [2/3] Building firmware...
"%PYTHON_EXE%" "%IDF_PY%" build
if errorlevel 1 goto error

set "PORT=%~1"
if "%PORT%"=="" (
    set /p PORT="Enter your ESP32-S3 COM port (e.g. COM4) or press Enter to skip flash: "
)

if not "%PORT%"=="" (
    echo [3/3] Flashing to %PORT% and opening monitor...
    "%PYTHON_EXE%" "%IDF_PY%" -p %PORT% flash monitor
) else (
    echo [SUCCESS] Build succeeded! To flash later:
    echo   idf.py -p COMx flash monitor
)

goto end

:error
echo [ERROR] Build failed. Please inspect logs above.
pause

:end
endlocal

