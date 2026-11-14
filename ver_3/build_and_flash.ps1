param(
    [string]$Port = ""
)

# 1. Activate environment
. "$PSScriptRoot\setup_env.ps1"

# 2. Check if build directory or target needs configuration
if (-not (Test-Path "$PSScriptRoot\build")) {
    Write-Host "`n[STEP 1/3] Setting target to esp32s3..." -ForegroundColor Cyan
    idf.py set-target esp32s3
}

# 3. Build project
Write-Host "`n[STEP 2/3] Building speaker test project..." -ForegroundColor Cyan
idf.py build

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n[ERROR] Build failed! Check compiler output above." -ForegroundColor Red
    exit 1
}

# 4. Flash and Monitor if port is specified or detected
if ([string]::IsNullOrWhiteSpace($Port)) {
    # Try finding available COM ports
    $availablePorts = [System.IO.Ports.SerialPort]::GetPortNames() | Select-Object -Unique
    if ($availablePorts.Count -eq 1) {
        $Port = $availablePorts[0]
        Write-Host "`n[INFO] Auto-detected single COM port: $Port" -ForegroundColor Green
    } elseif ($availablePorts.Count -gt 1) {
        Write-Host "`nAvailable COM Ports: $($availablePorts -join ', ')" -ForegroundColor Yellow
        $Port = Read-Host "Enter COM port to flash (e.g. COM4) or press Enter to skip flashing"
    }
}

if (-not [string]::IsNullOrWhiteSpace($Port)) {
    Write-Host "`n[STEP 3/3] Flashing to $Port and starting monitor..." -ForegroundColor Cyan
    idf.py -p $Port flash monitor
} else {
    Write-Host "`n[SUCCESS] Build completed! To flash manually run:" -ForegroundColor Green
    Write-Host "  idf.py -p COMx flash monitor" -ForegroundColor Yellow
}

