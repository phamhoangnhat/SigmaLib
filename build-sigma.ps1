# Build SigmaLib.dll tu dong: cai toolchain neu thieu (VS2022 BuildTools + Qt 6.9.0 + CMake), build Release x64, deploy sang thu muc app.
# Chay: chuot phai PowerShell (Run as Administrator) -> powershell -ExecutionPolicy Bypass -File build-sigma.ps1
#Requires -RunAsAdministrator

$ErrorActionPreference = "Stop"
$Repo       = Split-Path -Parent $MyInvocation.MyCommand.Path
$QtRoot     = "C:\Qt"
$QtVer      = "6.9.0"
$QtKitDir   = Join-Path $QtRoot "$QtVer\msvc2022_64"
$BuildDir   = Join-Path $Repo "build-cmake"
$DeployDir  = "D:\02_Github_Repository\SigmaHnpFork\SigmaHnpFork"

function Refresh-Path {
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" +
                [System.Environment]::GetEnvironmentVariable("Path", "User")
}
function Has-Cmd($name) { [bool](Get-Command $name -ErrorAction SilentlyContinue) }

Write-Host "== 1/5 Kiem tra / cai Visual Studio 2022 BuildTools (workload C++) ==" -ForegroundColor Cyan
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs2022 = $null
if (Test-Path $vswhere) {
    # Chi chap nhan VS2022 (17.x) co toolset C++ — MSBuild cua VS2019 khong du
    $vs2022 = & $vswhere -latest -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
}
if (-not $vs2022) {
    Write-Host "Dang cai VS2022 BuildTools + VCTools + ATL (vai GB, khoang 10-25 phut)..."
    winget install --id Microsoft.VisualStudio.2022.BuildTools -e --accept-package-agreements --accept-source-agreements `
        --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.ATL --includeRecommended"
    if ($LASTEXITCODE -ne 0 -and $LASTEXITCODE -ne 3010) { throw "winget BuildTools that bai (exit $LASTEXITCODE)" }
    Refresh-Path
    $vs2022 = & $vswhere -latest -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vs2022) { throw "Khong tim thay VS2022 sau khi cai" }
}
Write-Host "VS2022: $vs2022"

Write-Host "== 2/5 Kiem tra / cai CMake va Python ==" -ForegroundColor Cyan
if (-not (Has-Cmd cmake))   { winget install --id Kitware.CMake -e --accept-package-agreements --accept-source-agreements; Refresh-Path }
if (-not (Has-Cmd python))  { winget install --id Python.Python.3.12 -e --accept-package-agreements --accept-source-agreements; Refresh-Path }
if (-not (Has-Cmd cmake))  { throw "cmake khong co tren PATH" }
if (-not (Has-Cmd python)) { throw "python khong co tren PATH" }

Write-Host "== 3/5 Kiem tra / cai Qt $QtVer (win64_msvc2022_64) ==" -ForegroundColor Cyan
if (-not (Test-Path (Join-Path $QtKitDir "bin\qmake.exe"))) {
    Write-Host "Dang tai Qt $QtVer (~1.5 GB)..."
    python -m pip install --upgrade aqtinstall
    if ($LASTEXITCODE -ne 0) { throw "pip aqtinstall that bai" }
    python -m aqt install-qt -O $QtRoot windows desktop $QtVer win64_msvc2022_64
    if ($LASTEXITCODE -ne 0) { throw "aqt install-qt that bai" }
}

Write-Host "== 4/5 Configure + build Release x64 ==" -ForegroundColor Cyan
cmake -S $Repo -B $BuildDir -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="$QtKitDir"
if ($LASTEXITCODE -ne 0) { throw "cmake configure that bai" }
cmake --build $BuildDir --config Release
if ($LASTEXITCODE -ne 0) { throw "cmake build that bai" }

$dll = Join-Path $BuildDir "Release\SigmaLib.dll"
if (-not (Test-Path $dll)) { throw "Khong thay DLL ket qua: $dll" }

Write-Host "== 5/5 Deploy ==" -ForegroundColor Cyan
Get-Process Sigma -ErrorAction SilentlyContinue | Stop-Process -Force
Copy-Item $dll $DeployDir -Force
Write-Host "Da deploy: $DeployDir\SigmaLib.dll" -ForegroundColor Green

Write-Host ""
Write-Host "HOAN TAT. Nho truoc khi test:" -ForegroundColor Yellow
Write-Host "  1. Tat 'Cap nhat tu dong' trong cau hinh Sigma (de updater khong ghi de DLL)."
Write-Host "  2. Khoi dong lai Sigma.exe (app chay admin)."
Write-Host "  3. Doi chien luoc test (khong can restart Sigma):"
Write-Host '     reg add "HKCU\Software\Sigma\Config" /v strategyFixModernEditor /t REG_DWORD /d 1 /f'
Write-Host "     (0=hanh vi cu, 1=tach lo+scancode, 2=backspace Unicode, 3=ket hop)"
