# Build script for NansOS
param(
    [string]$BuildType = "test",
    [string]$Version = "",
    [string]$Component = "all",  # Can be "all", "os", or "bootloader"
    [switch]$UpdateChangelog
)

# Get root directory
$ROOT_DIR = (Get-Item $PSScriptRoot).Parent.Parent.FullName

# Load version configuration
$versionConfig = Get-Content "$ROOT_DIR/version.conf" | Where-Object { $_ -match '=' } | ConvertFrom-StringData

# Directories
$SRC_DIR = "$ROOT_DIR/src"
$BIN_DIR = "$ROOT_DIR/bin"
$BUILD_DIR = "$ROOT_DIR/builds"
$CONFIG_DIR = "$SRC_DIR/config"
$KERNEL_DIR = "$SRC_DIR/kernel"
$DRIVERS_DIR = "$SRC_DIR/drivers"
$GUI_DIR = "$SRC_DIR/gui"
$BOOT_DIR = "$SRC_DIR/boot"

# Function to increment build number
function Update-BuildNumber {
    param(
        [string]$Component
    )
    
    $buildNumber = [int]$versionConfig.LAST_BUILD_NUMBER + 1
    $buildNumberStr = '{0:D3}' -f $buildNumber
    
    # Update specific component build number
    if ($Component -eq "os") {
        $versionConfig.OS_BUILD = $buildNumberStr
    } elseif ($Component -eq "bootloader") {
        $versionConfig.BOOTLOADER_BUILD = $buildNumberStr
    }
    
    # Update configuration file
    $newContent = @"
# NansOS Version Configuration

# OS Version Information
OS_VERSION=$($versionConfig.OS_VERSION)
OS_BUILD=$($versionConfig.OS_BUILD)

# Bootloader Version Information
BOOTLOADER_VERSION=$($versionConfig.BOOTLOADER_VERSION)
BOOTLOADER_BUILD=$($versionConfig.BOOTLOADER_BUILD)

# Build Information
BUILD_TYPE=$($BuildType)
LAST_BUILD_NUMBER=$buildNumberStr
"@
    $newContent | Set-Content "$ROOT_DIR/version.conf"
    return $buildNumberStr
}

# Function to update changelog
function Update-Changelog {
    param(
        [string]$Version,
        [string]$BuildType,
        [string]$Component
    )

    # Create changelog directory if it doesn't exist
    if (!(Test-Path "changelog")) {
        New-Item -ItemType Directory -Path "changelog" | Out-Null
    }

    # Create new changelog entry
    $date = Get-Date -Format "yyyy-MM-dd"
    $changelogEntry = @"

## [$Version] - $date
### Build Information
- Type: $BuildType
- Component: $Component
- OS Version: $($versionConfig.OS_VERSION) (Build $($versionConfig.OS_BUILD))
- Bootloader Version: $($versionConfig.BOOTLOADER_VERSION) (Build $($versionConfig.BOOTLOADER_BUILD))

### Changes
- 

"@

    # Create or update changelog file
    $changelogFile = "changelog/CHANGELOG.md"
    if (!(Test-Path $changelogFile)) {
        @"
# NansOS Changelog

All notable changes to NansOS will be documented in this file.
$changelogEntry
"@ | Out-File $changelogFile
    } else {
        $content = Get-Content $changelogFile
        $unreleased = $content | Select-String -Pattern "## \[Unreleased\]" -Context 0,1
        if ($unreleased) {
            $content = $content[0..($unreleased.LineNumber-1)] + $changelogEntry + $content[$unreleased.LineNumber..$content.Length]
        } else {
            $content = @("# NansOS Changelog", "", "All notable changes to NansOS will be documented in this file.", "") + $changelogEntry + $content
        }
        $content | Out-File $changelogFile
    }

    Write-Host "Changelog updated at: $changelogFile" -ForegroundColor Cyan
}

Write-Host "Building NansOS..." -ForegroundColor Green
Write-Host

# Check for required tools
$missingTools = $false

function Test-Command {
    param($command)
    if (!(Get-Command $command -ErrorAction SilentlyContinue)) {
        return $false
    }
    return $true
}

if (!(Test-Command "nasm")) {
    Write-Host "Error: NASM is not installed or not in PATH" -ForegroundColor Red
    Write-Host "Please install NASM from https://www.nasm.us/"
    $missingTools = $true
}

if (!(Test-Command "ld")) {
    Write-Host "Error: LD (GNU Linker) is not installed or not in PATH" -ForegroundColor Red
    Write-Host "Please install MinGW-w64 and add it to PATH"
    $missingTools = $true
}

if (!(Test-Command "qemu-system-x86_64")) {
    Write-Host "Warning: QEMU is not installed or not in PATH" -ForegroundColor Yellow
    Write-Host "You won't be able to test the OS without QEMU"
}

if ($missingTools) {
    Write-Host
    Write-Host "Please install the required tools and try again." -ForegroundColor Red
    exit 1
}

# Create directory structure
Write-Host "Creating directories..." -ForegroundColor Cyan
New-Item -ItemType Directory -Force -Path $SRC_DIR | Out-Null
New-Item -ItemType Directory -Force -Path $BIN_DIR | Out-Null
New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null
New-Item -ItemType Directory -Force -Path $CONFIG_DIR | Out-Null

# Determine version string and build type
$buildDate = Get-Date -Format "yyyy-MM-dd"
switch ($BuildType) {
    "release" {
        if ($Version) {
            switch ($Component) {
                "os" { 
                    $versionConfig.OS_VERSION = $Version
                    $buildVersion = "os-$Version"
                }
                "bootloader" {
                    $versionConfig.BOOTLOADER_VERSION = $Version
                    $buildVersion = "boot-$Version"
                }
                default {
                    $versionConfig.OS_VERSION = $Version
                    $versionConfig.BOOTLOADER_VERSION = $Version
                    $buildVersion = $Version
                }
            }
        } else {
            Write-Host "Error: Release builds require a version number" -ForegroundColor Red
            exit 1
        }
        $buildType = "Release"
        if ($UpdateChangelog) {
            Update-Changelog -Version $Version -BuildType $buildType -Component $Component
        }
    }
    "test" {
        $buildNumber = Update-BuildNumber -Component $Component
        $buildVersion = "test-$buildDate-$buildNumber"
        $buildType = "Test"
    }
    default {
        $buildNumber = Update-BuildNumber -Component $Component
        $buildVersion = "$buildDate-$buildNumber"
        $buildType = "Development"
    }
}

$versionDir = "$BUILD_DIR\$buildVersion"

Write-Host "Build Information:" -ForegroundColor Cyan
Write-Host "  Type: $buildType" -ForegroundColor Cyan
Write-Host "  Version: $buildVersion" -ForegroundColor Cyan
Write-Host "  OS Version: $($versionConfig.OS_VERSION) (Build $($versionConfig.OS_BUILD))" -ForegroundColor Cyan
Write-Host "  Bootloader Version: $($versionConfig.BOOTLOADER_VERSION) (Build $($versionConfig.BOOTLOADER_BUILD))" -ForegroundColor Cyan
Write-Host

# Clean previous build
Write-Host "Cleaning previous build..." -ForegroundColor Cyan
Remove-Item -Path "$BIN_DIR\*" -Force -Recurse -ErrorAction SilentlyContinue
Remove-Item -Path "$BUILD_DIR\nanos.img" -Force -ErrorAction SilentlyContinue

# Build bootloader
Write-Host "Building bootloader..." -ForegroundColor Cyan

# Build stage 1
Write-Host "  Building stage 1..." -ForegroundColor Cyan
nasm -f bin -i "$BOOT_DIR" "$BOOT_DIR/boot.asm" -o "$BIN_DIR/stage1.bin"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Stage 1 build failed!" -ForegroundColor Red
    exit 1
}

# Build stage 2
Write-Host "  Building stage 2..." -ForegroundColor Cyan
nasm -f bin -i "$BOOT_DIR" "$BOOT_DIR/stage2.asm" -o "$BIN_DIR/stage2.bin"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Stage 2 build failed!" -ForegroundColor Red
    exit 1
}

# Verify bootloader sizes
$stage1Size = (Get-Item "$BIN_DIR/stage1.bin").Length
$stage2Size = (Get-Item "$BIN_DIR/stage2.bin").Length
Write-Host "  Stage 1 size: $stage1Size bytes" -ForegroundColor Green
Write-Host "  Stage 2 size: $stage2Size bytes" -ForegroundColor Green

if ($stage1Size -gt 512) {
    Write-Host "Error: Stage 1 exceeds 512 bytes!" -ForegroundColor Red
    exit 1
}

if ($stage2Size -gt 1024) {
    Write-Host "Error: Stage 2 exceeds 1024 bytes!" -ForegroundColor Red
    exit 1
}

# Build kernel components
Write-Host "Building kernel components..." -ForegroundColor Cyan

# Build entry point
Write-Host "  Building entry point..." -ForegroundColor Cyan
nasm -f elf32 "$KERNEL_DIR/entry.asm" -o "$BIN_DIR/entry.o"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Entry point build failed!" -ForegroundColor Red
    exit 1
}

# Build main kernel
Write-Host "  Building main kernel..." -ForegroundColor Cyan
nasm -f elf32 "$KERNEL_DIR/kernel.asm" -o "$BIN_DIR/kernel.o"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Kernel build failed!" -ForegroundColor Red
    exit 1
}

# Build utilities
Write-Host "  Building utilities..." -ForegroundColor Cyan
nasm -f elf32 "$KERNEL_DIR/utils.asm" -o "$BIN_DIR/utils.o"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Utilities build failed!" -ForegroundColor Red
    exit 1
}

# Build memory manager
Write-Host "  Building memory manager..." -ForegroundColor Cyan
nasm -f elf32 "$KERNEL_DIR/memory.asm" -o "$BIN_DIR/memory.o"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Memory manager build failed!" -ForegroundColor Red
    exit 1
}

# Build system info
Write-Host "  Building system info..." -ForegroundColor Cyan
nasm -f elf32 "$KERNEL_DIR/sysinfo.asm" -o "$BIN_DIR/sysinfo.o"
if ($LASTEXITCODE -ne 0) {
    Write-Host "System info build failed!" -ForegroundColor Red
    exit 1
}

# Build drivers
Write-Host "  Building drivers..." -ForegroundColor Cyan
Get-ChildItem "$DRIVERS_DIR/*.asm" | ForEach-Object {
    $outFile = "$BIN_DIR/" + $_.BaseName + ".o"
    nasm -f elf32 $_.FullName -o $outFile
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Driver build failed: $($_.Name)" -ForegroundColor Red
        exit 1
    }
}

# Build GUI components
Write-Host "  Building GUI components..." -ForegroundColor Cyan
Get-ChildItem "$GUI_DIR/*.asm" | ForEach-Object {
    $outFile = "$BIN_DIR/" + $_.BaseName + ".o"
    nasm -f elf32 $_.FullName -o $outFile
    if ($LASTEXITCODE -ne 0) {
        Write-Host "GUI component build failed: $($_.Name)" -ForegroundColor Red
        exit 1
    }
}

# Link kernel
Write-Host "Linking kernel..." -ForegroundColor Cyan
$objFiles = Get-ChildItem "$BIN_DIR/*.o" | ForEach-Object { $_.FullName }

# Link with 32-bit flags for Windows
ld -m i386pe `
   -T "$CONFIG_DIR/linker.ld" `
   --oformat binary `
   -o "$BIN_DIR/kernel.bin" `
   $objFiles

if ($LASTEXITCODE -ne 0) {
    Write-Host "Kernel linking failed!" -ForegroundColor Red
    exit 1
}

# Verify kernel binary
if (!(Test-Path "$BIN_DIR/kernel.bin")) {
    Write-Host "Error: Kernel binary not created!" -ForegroundColor Red
    exit 1
}

$kernelSize = (Get-Item "$BIN_DIR/kernel.bin").Length
Write-Host "Kernel binary size: $kernelSize bytes" -ForegroundColor Green

# Create disk image
Write-Host "Creating disk image..." -ForegroundColor Cyan
$diskSize = 10485760  # 10 MB

# Create empty disk image
$image = [byte[]]::new($diskSize)

# Load bootloader stages and kernel
Write-Host "Writing bootloader..." -ForegroundColor Cyan
try {
    $stage1 = [System.IO.File]::ReadAllBytes("$BIN_DIR/stage1.bin")
    $stage2 = [System.IO.File]::ReadAllBytes("$BIN_DIR/stage2.bin")
    $kernel = [System.IO.File]::ReadAllBytes("$BIN_DIR/kernel.bin")

    # Copy stage 1 (sector 0)
    [Array]::Copy($stage1, 0, $image, 0, $stage1.Length)
    
    # Copy stage 2 (sectors 1-2)
    [Array]::Copy($stage2, 0, $image, 512, $stage2.Length)
    
    # Copy kernel (sectors 3+)
    [Array]::Copy($kernel, 0, $image, 1536, $kernel.Length)

    # Write complete image
    [System.IO.File]::WriteAllBytes("$BUILD_DIR/nanos.img", $image)
    Write-Host "Disk image created successfully: $((Get-Item "$BUILD_DIR/nanos.img").Length) bytes" -ForegroundColor Green
    
    # Show layout
    Write-Host "Disk layout:" -ForegroundColor Cyan
    Write-Host "  Sector 0:    Stage 1 ($stage1Size bytes)" -ForegroundColor Green
    Write-Host "  Sectors 1-2: Stage 2 ($stage2Size bytes)" -ForegroundColor Green
    Write-Host "  Sectors 3+:  Kernel ($($kernel.Length) bytes)" -ForegroundColor Green
}
catch {
    Write-Host "Error creating disk image: $_" -ForegroundColor Red
    exit 1
}

# Create version directory
Write-Host "Creating build directory..." -ForegroundColor Cyan
New-Item -ItemType Directory -Force -Path $versionDir | Out-Null

# Move build files
Write-Host "Moving build files..." -ForegroundColor Cyan
Move-Item "$BUILD_DIR/nanos.img" "$versionDir/nanos.img" -Force

# Create build info
Write-Host "Creating build info..." -ForegroundColor Cyan
@"
NansOS Build Information
------------------------
Build Version: $buildVersion
Build Type: $buildType
Build Component: $Component

OS Information:
  Version: $($versionConfig.OS_VERSION)
  Build: $($versionConfig.OS_BUILD)

Bootloader Information:
  Version: $($versionConfig.BOOTLOADER_VERSION)
  Build: $($versionConfig.BOOTLOADER_BUILD)

Build Details:
  Date: $buildDate
  Time: $(Get-Date -Format "HH:mm")
  Architecture: x86_64
"@ | Out-File "$versionDir/build_info.txt"

# Create current build link
Write-Host "Creating current build link..." -ForegroundColor Cyan
Copy-Item "$versionDir/nanos.img" "$BUILD_DIR/nanos.img" -Force

Write-Host
Write-Host "Build successful!" -ForegroundColor Green
Write-Host "Build location: $versionDir"
Write-Host
Write-Host "To run NansOS in QEMU, type: qemu-system-x86_64 -fda $BUILD_DIR/nanos.img" -ForegroundColor Cyan
Write-Host 