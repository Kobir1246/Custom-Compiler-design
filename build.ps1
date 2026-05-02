<#
  build.ps1 - robust build script for the MiniC project on Windows (PowerShell).
  Usage:
    powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
  Or:
    .\build.ps1
#>

[CmdletBinding()]
param(
    [string]$SourceDir = ".",
    [string]$OutExe = "mycompiler.exe"
)

$ErrorActionPreference = "Stop"

function Find-Tool {
    param(
        [string]$name,
        [string]$envvar
    )

    # 1) If an environment variable with this name exists, return its value
    $envItem = Get-Item -Path ("Env:" + $envvar) -ErrorAction SilentlyContinue
    if ($envItem) {
        $val = $envItem.Value
        if ($val -and (Test-Path $val -PathType Leaf -ErrorAction SilentlyContinue)) {
            return $val
        }
        if ($val) { return $val }
    }

    # 2) Otherwise try to locate the tool in PATH
    $cmd = Get-Command $name -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Path }

    return $null
}

Write-Host "Building in: " (Resolve-Path $SourceDir) -ForegroundColor Cyan

# Tools (can override by setting env vars BISON,FLEX,GCC)
$bison = Find-Tool "bison" "BISON"
$flex  = Find-Tool "flex"  "FLEX"
$gcc   = Find-Tool "gcc"   "GCC"

if (-not $bison) {
    Write-Error "bison not found. Install bison or set environment variable BISON to path."
    exit 1
}
if (-not $flex) {
    Write-Error "flex not found. Install flex or set environment variable FLEX to path."
    exit 1
}
if (-not $gcc) {
    Write-Error "gcc not found. Install MinGW/MSYS or set environment variable GCC to path."
    exit 1
}

Write-Host ("Using: bison=" + $bison + ", flex=" + $flex + ", gcc=" + $gcc) -ForegroundColor Green

Push-Location $SourceDir

try {
    # 1) Bison
    Write-Host ""
    Write-Host "[1/4] Running bison -d parser.y ..." -ForegroundColor Yellow
    & $bison -d parser.y 2>&1 | ForEach-Object { Write-Host $_ }
    if ($LASTEXITCODE -ne 0) { throw "bison failed (exit " + $LASTEXITCODE + ")" }

    # 2) Flex
    Write-Host ""
    Write-Host "[2/4] Running flex lexer.l ..." -ForegroundColor Yellow
    & $flex lexer.l 2>&1 | ForEach-Object { Write-Host $_ }
    if ($LASTEXITCODE -ne 0) { throw "flex failed (exit " + $LASTEXITCODE + ")" }

    # 3) Prepare compile list
    $sources = @("parser.tab.c", "lex.yy.c", "codegen.c", "main.c", "symbol_table.c", "scanner_bridge.c")
    $existing = $sources | Where-Object { Test-Path $_ }
    Write-Host ""
    Write-Host "Found source files to compile:" -ForegroundColor Cyan
    $existing | ForEach-Object { Write-Host "  " + $_ }

    if (-not $existing) { throw "No source files found to compile." }

    # 4) Compile
    Write-Host ""
    Write-Host "[3/4] Compiling with gcc ..." -ForegroundColor Yellow
    $compileArgs = @("-o", $OutExe) + $existing + @("-g")
    Write-Host ("gcc " + ($compileArgs -join ' ')) -ForegroundColor Gray
    & $gcc @compileArgs 2>&1 | ForEach-Object { Write-Host $_ }
    if ($LASTEXITCODE -ne 0) { throw "gcc failed (exit " + $LASTEXITCODE + ")" }

    Write-Host ""
    Write-Host ("[4/4] Build succeeded. Output: " + $OutExe) -ForegroundColor Green
}
catch {
    $errText = [Environment]::NewLine + "Build failed: " + $_.ToString()
    Write-Error $errText
    Pop-Location
    exit 1
}

Pop-Location
exit 0
