# NansOS Build System Documentation

## Overview

NansOS uses a PowerShell-based build system that handles compilation, linking, and disk image creation. The build system is designed to be modular, extensible, and user-friendly.

## Prerequisites

### Required Tools
- NASM (Netwide Assembler) v2.15 or later
- MinGW-w64 with GNU LD
- QEMU for system emulation
- PowerShell 5.1 or later
- Git for version control

### Installation

1. **NASM**
   ```powershell
   # Using Chocolatey
   choco install nasm
   # Or download from https://www.nasm.us/
   ```

2. **MinGW-w64**
   ```powershell
   # Using Chocolatey
   choco install mingw
   # Or download from https://www.mingw-w64.org/
   ```

3. **QEMU**
   ```powershell
   # Using Chocolatey
   choco install qemu
   # Or download from https://www.qemu.org/
   ```

## Build Process

### Directory Structure
```
NansOS/
├── src/
│   ├── boot/          # Bootloader source
│   │   ├── stage1.asm # Stage 1 bootloader
│   │   ├── stage2.asm # Stage 2 bootloader
│   │   └── common.inc # Shared bootloader code
│   ├── kernel/        # Kernel components
│   ├── drivers/       # Hardware drivers
│   ├── gui/           # Interface components
│   └── config/        # Build configuration
├── bin/               # Build artifacts
├── builds/            # Final disk images
└── tools/             # Build tools
```

### Build Scripts

1. **build.ps1**
   - Main build script
   - Handles compilation and linking
   - Creates disk image
   ```powershell
   # Standard build
   .\build.ps1
   
   # Release build
   .\build.ps1 -BuildType release -Version "1.0.0"
   
   # Clean build
   .\build.ps1 -Clean
   ```

2. **run.bat**
   - Launches QEMU with correct parameters
   - Handles debugging configuration
   ```batch
   # Run OS
   run.bat
   
   # Debug mode
   run.bat debug
   
   # Build only
   run.bat build
   ```

## Build Configuration

### Version Control
- `version.conf` controls versioning
- Tracks OS, bootloader stage 1, and stage 2 versions
- Automatically increments build numbers

### Build Types

1. **Test Build**
   - Default build type
   - Includes debug information
   - Faster compilation

2. **Release Build**
   - Optimized for performance
   - Strips debug information
   - Creates release artifacts

3. **Debug Build**
   - Includes debug symbols
   - Enables GDB support
   - Additional logging

## Build Steps

1. **Compilation**
   ```powershell
   # Stage 1 Bootloader
   nasm -f bin src/boot/stage1.asm -o bin/stage1.bin
   
   # Stage 2 Bootloader
   nasm -f bin src/boot/stage2.asm -o bin/stage2.bin
   
   # Kernel components
   nasm -f elf64 src/kernel/*.asm -o bin/*.o
   ```

2. **Linking**
   ```powershell
   # Link kernel
   ld -m i386pep -T src/config/linker.ld -o bin/kernel.bin *.o
   ```

3. **Disk Image Creation**
   - Creates 1.44MB floppy image
   - Sector 0: Stage 1 bootloader (512 bytes)
   - Sectors 1-2: Stage 2 bootloader (1024 bytes)
   - Sectors 3+: Kernel
   - Verifies component sizes

## Testing

### QEMU Testing
```powershell
# Basic testing
qemu-system-x86_64 -fda builds/nanos.img

# With debugging
qemu-system-x86_64 -fda builds/nanos.img -s -S
```

### GDB Debugging
```bash
# Connect to QEMU
gdb
target remote localhost:1234
```

## Common Issues

### Build Errors

1. **Bootloader Size Errors**
   ```
   Stage 1 exceeds 512 bytes
   Stage 2 exceeds 1024 bytes
   ```
   - Stage 1 must fit in one sector
   - Stage 2 must fit in two sectors
   - Optimize code or move functionality

2. **Section Base Warnings**
   ```
   section below image base
   ```
   - Normal for bootable kernel
   - Can be safely ignored

### Runtime Errors

1. **Boot Failures**
   - Check stage 1 size
   - Verify stage 2 loading address
   - Check GDT setup

2. **Kernel Panics**
   - Use GDB for debugging
   - Check memory mapping
   - Verify interrupt setup

## Build Customization

### Custom Builds
```powershell
# Component-specific build
.\build.ps1 -Component bootloader

# Custom version
.\build.ps1 -Version "1.1.0-alpha"
```

### Environment Variables
```powershell
# Set build environment
$env:NANOS_DEBUG = 1
$env:NANOS_OPTIMIZE = 1
```

## Continuous Integration

### GitHub Actions
```yaml
name: NansOS Build
on: [push, pull_request]
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: .\build.ps1
```

## References
- [Build Script Reference](build.ps1)
- [Linker Script Documentation](src/config/linker.ld)
- [Version Configuration](version.conf) 