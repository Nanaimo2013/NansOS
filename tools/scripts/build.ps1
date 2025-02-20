# NansOS Build Script
# Builds the operating system and creates bootable disk image

param(
    [string]$BuildType = "test",    # Build type: test, release, debug
    [string]$Version = "",          # Version override
    [switch]$Clean                  # Clean build
)

# Configuration
$ErrorActionPreference = "Stop"
$BuildDir = "builds"
$BinDir = "bin"
$SrcDir = "src"

# Find MSYS2 installation
$msys2Path = "E:\msys64"  # Default MSYS2 path
$possiblePaths = @(
    "C:\msys64",
    "D:\msys64",
    "E:\msys64",
    "${env:ProgramFiles}\msys64",
    "${env:ProgramFiles(x86)}\msys64"
)

foreach ($path in $possiblePaths) {
    if (Test-Path $path) {
        $msys2Path = $path
        break
    }
}

# Set up tool paths
$env:Path = "$msys2Path\mingw64\bin;$env:Path"

# Ensure we're in the project root
if (-not (Test-Path "version.conf")) {
    Write-Error "Must run from project root directory"
    exit 1
}

# Create directories
function EnsureDirectories {
    @($BuildDir, $BinDir) | ForEach-Object {
        if (-not (Test-Path $_)) {
            New-Item -ItemType Directory -Path $_ | Out-Null
        }
    }
}

# Clean build artifacts
function CleanBuild {
    Write-Host "Cleaning build artifacts..." -ForegroundColor Yellow
    Remove-Item -Path "$BuildDir/*" -Force -Recurse -ErrorAction SilentlyContinue
    Remove-Item -Path "$BinDir/*" -Force -Recurse -ErrorAction SilentlyContinue
}

# Get version info
function GetVersion {
    if ($Version) {
        return $Version
    }
    
    $versionFile = Get-Content "version.conf"
    $version = $versionFile | Where-Object { $_ -match "OS_VERSION=" } | ForEach-Object { $_.Split('=')[1] }
    $build = $versionFile | Where-Object { $_ -match "OS_BUILD=" } | ForEach-Object { $_.Split('=')[1] }
    return "$version-$build"
}

# Compile assembly file
function CompileAsm {
    param($Source, $Target, $Format = "bin")
    
    Write-Host "Compiling $Source..." -ForegroundColor Green
    
    # Use appropriate format for kernel files
    if ($Source -match "kernel|drivers|gui") {
        $Format = "elf64"
    }
    
    nasm -f $Format $Source -o $Target
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to compile $Source"
        exit 1
    }
}

# Link object files
function LinkObjects {
    param($Objects, $Target, $Script = "src/config/linker.ld")
    
    Write-Host "Linking $Target..." -ForegroundColor Green
    
    # Create temporary executable
    $tempExe = "$BinDir/kernel.exe"
    
    # Use MinGW-w64 toolchain
    $linker = "ld"
    $objcopy = "objcopy"
    
    # Link objects into ELF executable
    & $linker -m i386pep --oformat pei-x86-64 -T $Script -o $tempExe $Objects
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to link $Target"
        exit 1
    }
    
    # Convert to flat binary
    & $objcopy -O binary -j .text -j .data -j .bss $tempExe $Target
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to create binary $Target"
        exit 1
    }
    
    # Clean up temporary file
    Remove-Item $tempExe -ErrorAction SilentlyContinue
}

# Create disk image
function CreateDiskImage {
    param($Target)
    
    Write-Host "Creating disk image $Target..." -ForegroundColor Green
    
    # Create empty image (1.44MB - standard floppy size)
    $imageSize = 1474560  # 1.44MB in bytes
    $block = New-Object byte[] 512
    $stream = [System.IO.File]::Create($Target)
    
    try {
        # Fill with zeros
        for ($i = 0; $i -lt ($imageSize / 512); $i++) {
            $stream.Write($block, 0, 512)
        }
    }
    finally {
        $stream.Close()
    }
    
    # Write bootloader (sector 0)
    Write-Host "Writing bootloader..." -ForegroundColor Green
    $bootloader = [System.IO.File]::ReadAllBytes("$BinDir/stage1.bin")
    if ($bootloader.Length -gt 512) {
        Write-Error "Bootloader exceeds 512 bytes!"
        exit 1
    }
    $stream = [System.IO.File]::OpenWrite($Target)
    try {
        $stream.Write($bootloader, 0, $bootloader.Length)
        
        # Write boot signature if not present
        if ($bootloader.Length -lt 510) {
            $stream.Position = 510
            $signature = [byte[]]@(0x55, 0xAA)
            $stream.Write($signature, 0, 2)
        }
    }
    finally {
        $stream.Close()
    }
    
    # Write stage2 (sector 1)
    Write-Host "Writing stage2..." -ForegroundColor Green
    $stage2 = [System.IO.File]::ReadAllBytes("$BinDir/stage2.bin")
    $stream = [System.IO.File]::OpenWrite($Target)
    try {
        $stream.Position = 512  # Sector 1
        $stream.Write($stage2, 0, $stage2.Length)
    }
    finally {
        $stream.Close()
    }
    
    # Write kernel (sector 4)
    Write-Host "Writing kernel..." -ForegroundColor Green
    $kernel = [System.IO.File]::ReadAllBytes("$BinDir/kernel.bin")
    $stream = [System.IO.File]::OpenWrite($Target)
    try {
        $stream.Position = 512 * 4  # Sector 4
        $stream.Write($kernel, 0, $kernel.Length)
    }
    finally {
        $stream.Close()
    }
    
    Write-Host "Disk image created successfully" -ForegroundColor Green
}

# Main build process
try {
    # Initial setup
    Write-Host "Building NansOS $(GetVersion) ($BuildType)..." -ForegroundColor Cyan
    EnsureDirectories
    
    if ($Clean) {
        CleanBuild
    }
    
    # Compile kernel entry
    Write-Host "Compiling kernel entry..."
    $entryPath = Join-Path $SrcDir "kernel/entry.asm"
    $entryOutput = Join-Path $BinDir "entry.o"
    nasm -f elf32 $entryPath -o $entryOutput
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed: Failed to compile kernel entry"
        exit 1
    }
    
    # Compile other assembly files
    Get-ChildItem -Path (Join-Path $SrcDir "kernel/*.asm") -Exclude "entry.asm" | ForEach-Object {
        Write-Host "Compiling $($_.FullName)..."
        $output = Join-Path $BinDir "$($_.BaseName).o"
        nasm -f elf32 $_.FullName -o $output
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Build failed: Failed to compile $($_.Name)"
            exit 1
        }
    }
    
    Get-ChildItem -Path (Join-Path $SrcDir "drivers/*.asm") | ForEach-Object {
        Write-Host "Compiling $($_.FullName)..."
        $output = Join-Path $BinDir "$($_.BaseName).o"
        nasm -f elf32 $_.FullName -o $output
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Build failed: Failed to compile $($_.Name)"
            exit 1
        }
    }
    
    Get-ChildItem -Path (Join-Path $SrcDir "gui/*.asm") | ForEach-Object {
        Write-Host "Compiling $($_.FullName)..."
        $output = Join-Path $BinDir "$($_.BaseName).o"
        nasm -f elf32 $_.FullName -o $output
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Build failed: Failed to compile $($_.Name)"
            exit 1
        }
    }
    
    # Link kernel
    Write-Host "Linking kernel..."
    $linkerScript = Join-Path $SrcDir "linker.ld"
    $kernelOutput = Join-Path $BuildDir "kernel.elf"
    $objFiles = Get-ChildItem -Path (Join-Path $BinDir "*.o") | ForEach-Object { $_.FullName }
    i386-elf-ld -T $linkerScript -o $kernelOutput $objFiles
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed: Failed to link kernel"
        exit 1
    }
    
    # Create disk image
    CreateDiskImage -Target "$BuildDir/nanos.img"
    
    # Copy to test directory if test build
    if ($BuildType -eq "test") {
        Copy-Item "$BuildDir/nanos.img" "testing/nanos.img" -Force
    }
    
    Write-Host "Build completed successfully!" -ForegroundColor Green
}
catch {
    Write-Error "Build failed: $_"
    exit 1
} 