# Root build script for NansOS
# This script forwards all parameters to the main build script in tools/scripts

$scriptPath = Join-Path $PSScriptRoot "tools/scripts/build.ps1"
if (!(Test-Path $scriptPath)) {
    Write-Host "Error: Build script not found at: $scriptPath" -ForegroundColor Red
    exit 1
}

# Create tools/scripts directory if it doesn't exist
$scriptsDir = Join-Path $PSScriptRoot "tools/scripts"
if (!(Test-Path $scriptsDir)) {
    New-Item -ItemType Directory -Force -Path $scriptsDir | Out-Null
}

# Forward all arguments to the main build script
& $scriptPath @args 