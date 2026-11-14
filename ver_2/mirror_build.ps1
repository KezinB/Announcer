<#
.SYNOPSIS
    Mirror ver_2 to a short local path and build / flash from there.
    Google Drive paths are too long for some ESP-IDF object files (Windows MAX_PATH),
    and build trees should not sync to Drive.
.EXAMPLE
    .\mirror_build.ps1                # sync + build
    .\mirror_build.ps1 -Port COM6     # sync + build + flash
    .\mirror_build.ps1 -Port COM6 -Monitor
#>
param(
    [string]$Port = "",
    [switch]$Monitor,
    [string]$Mirror = "$env:USERPROFILE\Downloads\Announcer_v2"
)

$src = $PSScriptRoot

# Mirror sources; keep the mirror's build/ and generated sdkconfig between runs
robocopy $src $Mirror /MIR /XD build .git /XF sdkconfig sdkconfig.old /NFL /NDL /NJH /NJS /NP | Out-Null
if ($LASTEXITCODE -ge 8) { Write-Host "[FAIL] robocopy error $LASTEXITCODE" -ForegroundColor Red; exit 1 }
Write-Host "[DONE] Mirrored to $Mirror" -ForegroundColor Green

Set-Location $Mirror
. "$Mirror\setup_env.ps1" | Out-Null

if (-not (Test-Path "$Mirror\build")) { idf.py set-target esp32s3 }
idf.py build
if ($LASTEXITCODE -ne 0) { Write-Host "[FAIL] Build failed" -ForegroundColor Red; exit 1 }
Write-Host "[PASS] Build OK" -ForegroundColor Green

if ($Port) {
    if ($Monitor) { idf.py -p $Port flash monitor } else { idf.py -p $Port flash }
}
