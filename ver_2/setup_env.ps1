<#
.SYNOPSIS
    Activates the ESP-IDF v6.0.2 environment in PowerShell.
.EXAMPLE
    . .\setup_env.ps1
    idf.py build
#>

$env:IDF_PATH = "C:\esp\v6.0.2\esp-idf"
$env:IDF_TOOLS_PATH = "C:\Espressif\tools"
$env:IDF_PYTHON_ENV_PATH = "C:\Espressif\tools\python\v6.0.2\venv"
$env:ESP_IDF_VERSION = "6.0"

$toolsPath = @(
    "C:\Espressif\tools\ccache\4.12.1\ccache-4.12.1-windows-x86_64",
    "C:\Espressif\tools\cmake\4.0.3\bin",
    "C:\Espressif\tools\ninja\1.12.1",
    "C:\Espressif\tools\idf-exe\1.0.3",
    "C:\Espressif\tools\xtensa-esp-elf\esp-15.2.0_20251204\xtensa-esp-elf\bin",
    "C:\Espressif\tools\python\v6.0.2\venv\Scripts"
) -join ";"

$env:PATH = "$toolsPath;$env:PATH"

function global:idf.py {
    & "C:\Espressif\tools\python\v6.0.2\venv\Scripts\python.exe" "C:\esp\v6.0.2\esp-idf\tools\idf.py" @args
}

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "   ESP-IDF v6.0.2 Environment Ready for ESP32-S3" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "IDF_PATH:        $env:IDF_PATH"
Write-Host "Target:          esp32s3"
Write-Host ""
Write-Host "Quick Commands:" -ForegroundColor Yellow
Write-Host "  idf.py build                -> Compile project"
Write-Host "  idf.py -p COMx flash        -> Flash to board (e.g. COM4)"
Write-Host "  idf.py -p COMx monitor      -> Open interactive speaker test monitor"
Write-Host "  idf.py -p COMx flash monitor-> Flash & launch monitor in one command"
Write-Host "============================================================" -ForegroundColor Cyan

