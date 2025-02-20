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

# NansOS Build Script for GRUB

# Version info
$VERSION = "1.0.0-001"
$CONFIG = "test"

Write-Host "Building NansOS $VERSION ($CONFIG)..."

# Clean if requested
if ($args -contains "-Clean") {
    Write-Host "Cleaning build artifacts..."
    Remove-Item -Path "bin/*" -Force -Recurse -ErrorAction SilentlyContinue
    Remove-Item -Path "builds/*" -Force -Recurse -ErrorAction SilentlyContinue
    New-Item -ItemType Directory -Path "bin" -Force | Out-Null
    New-Item -ItemType Directory -Path "builds" -Force | Out-Null
}

# Create directories if they don't exist
if (-not (Test-Path "bin")) { New-Item -ItemType Directory -Path "bin" | Out-Null }
if (-not (Test-Path "builds")) { New-Item -ItemType Directory -Path "builds" | Out-Null }
if (-not (Test-Path "builds/boot/grub")) { 
    New-Item -ItemType Directory -Path "builds/boot/grub" -Force | Out-Null 
}

# Compile kernel entry
Write-Host "Compiling kernel entry..."
nasm -f elf32 src/kernel/entry.asm -o bin/entry.o
if ($LASTEXITCODE -ne 0) { 
    Write-Error "Failed to compile kernel entry"
    exit 1 
}

# Compile other assembly files
Get-ChildItem -Path "src/kernel/*.asm" -Exclude "entry.asm" | ForEach-Object {
    Write-Host "Compiling $($_.FullName)..."
    nasm -f elf32 $_.FullName -o "bin/$($_.BaseName).o"
    if ($LASTEXITCODE -ne 0) { 
        Write-Error "Failed to compile $($_.Name)"
        exit 1 
    }
}

Get-ChildItem -Path "src/drivers/*.asm" | ForEach-Object {
    Write-Host "Compiling $($_.FullName)..."
    nasm -f elf32 $_.FullName -o "bin/$($_.BaseName).o"
    if ($LASTEXITCODE -ne 0) { 
        Write-Error "Failed to compile $($_.Name)"
        exit 1 
    }
}

Get-ChildItem -Path "src/gui/*.asm" | ForEach-Object {
    Write-Host "Compiling $($_.FullName)..."
    nasm -f elf32 $_.FullName -o "bin/$($_.BaseName).o"
    if ($LASTEXITCODE -ne 0) { 
        Write-Error "Failed to compile $($_.Name)"
        exit 1 
    }
}

# Link kernel
Write-Host "Linking kernel..."
$ObjFiles = Get-ChildItem -Path "bin/*.o" | ForEach-Object { $_.FullName }
i386-elf-ld -T src/linker.ld -o builds/boot/kernel.elf $ObjFiles
if ($LASTEXITCODE -ne 0) { 
    Write-Error "Failed to link kernel"
    exit 1 
}

# Copy GRUB configuration
Write-Host "Setting up GRUB..."
Copy-Item "src/boot/grub/grub.cfg" "builds/boot/grub/" -Force

Write-Host "Build completed successfully!" 