# NansOS Build System Documentation

<div align="center">

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/actions)
[![PowerShell](https://img.shields.io/badge/PowerShell-5.1+-blue.svg?style=for-the-badge)](https://github.com/PowerShell/PowerShell)
[![NASM](https://img.shields.io/badge/NASM-2.15+-orange.svg?style=for-the-badge)](https://www.nasm.us/)

</div>

## 🚀 Quick Start

```powershell
# Clone repository
git clone https://github.com/Nanaimo2013/NansOS.git
cd NansOS

# Build OS
.\build.ps1

# Run in QEMU
.\run.bat
```

## 📋 Prerequisites

<table>
<tr>
<td width="50%">

### 🛠️ Required Tools
[![Tools](https://img.shields.io/badge/Tools-Required-red.svg)](https://github.com/Nanaimo2013/NansOS)

- NASM v2.15+
- MinGW-w64 (GNU LD)
- QEMU
- PowerShell 5.1+
- Git

</td>
<td width="50%">

### 📥 Installation
```powershell
# Using Chocolatey
choco install nasm
choco install mingw
choco install qemu
```

</td>
</tr>
</table>

## 📁 Project Structure

<table>
<tr>
<td width="50%">

### 📂 Directory Layout
```
NansOS/
├── src/               # Source code
│   ├── boot/         # Bootloader
│   ├── kernel/       # Kernel
│   ├── drivers/      # Drivers
│   └── gui/          # Interface
├── docs/             # Documentation
├── tools/            # Dev tools
└── builds/           # Build output
```

</td>
<td width="50%">

### 🔧 Key Components
[![Components](https://img.shields.io/badge/Components-Core-blue.svg)](https://github.com/Nanaimo2013/NansOS)

- **Bootloader**: Stage 1 & 2
- **Kernel**: Core system
- **Drivers**: Hardware support
- **GUI**: User interface
- **Tools**: Build scripts

</td>
</tr>
</table>

## 🏗️ Build Process

### 📜 Build Scripts

<table>
<tr>
<td width="50%">

#### build.ps1
[![Script](https://img.shields.io/badge/Script-Main-green.svg)](https://github.com/Nanaimo2013/NansOS)
```powershell
# Standard build
.\build.ps1

# Release build
.\build.ps1 -BuildType release -Version "1.0.0"

# Clean build
.\build.ps1 -Clean
```

</td>
<td width="50%">

#### run.bat
[![Script](https://img.shields.io/badge/Script-Runner-blue.svg)](https://github.com/Nanaimo2013/NansOS)
```batch
# Run OS
run.bat

# Debug mode
run.bat debug

# Build only
run.bat build
```

</td>
</tr>
</table>

## ⚙️ Build Configuration

<table>
<tr>
<td width="33%">

### 📝 Version Control
[![Version](https://img.shields.io/badge/Version-Managed-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- `version.conf`
- Build numbering
- Component versions

</td>
<td width="33%">

### 🎯 Build Types
[![Types](https://img.shields.io/badge/Types-Multiple-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Test (default)
- Release
- Debug

</td>
<td width="33%">

### 📊 Output
[![Output](https://img.shields.io/badge/Output-Managed-orange.svg)](https://github.com/Nanaimo2013/NansOS)
- Disk images
- Debug symbols
- Build info

</td>
</tr>
</table>

## 🔨 Build Steps

<table>
<tr>
<td width="50%">

### 1️⃣ Compilation
```powershell
# Stage 1 Bootloader
nasm -f bin src/boot/stage1.asm -o bin/stage1.bin

# Stage 2 Bootloader
nasm -f bin src/boot/stage2.asm -o bin/stage2.bin

# Kernel components
nasm -f elf64 src/kernel/*.asm -o bin/*.o
```

</td>
<td width="50%">

### 2️⃣ Linking
```powershell
# Link kernel
ld -m i386pep -T src/config/linker.ld -o bin/kernel.bin *.o
```

</td>
</tr>
<tr>
<td width="50%">

### 3️⃣ Image Creation
- 1.44MB floppy image
- Sector 0: Stage 1 (512B)
- Sectors 1-2: Stage 2 (1KB)
- Sectors 3+: Kernel

</td>
<td width="50%">

### 4️⃣ Verification
- Size checks
- Signature validation
- Boot sector test
- Component integrity

</td>
</tr>
</table>

## 🧪 Testing

<table>
<tr>
<td width="50%">

### 🖥️ QEMU Testing
[![QEMU](https://img.shields.io/badge/QEMU-Supported-green.svg)](https://github.com/Nanaimo2013/NansOS)
```powershell
# Basic testing
qemu-system-x86_64 -fda builds/nanos.img

# With debugging
qemu-system-x86_64 -fda builds/nanos.img -s -S
```

</td>
<td width="50%">

### 🐛 GDB Debugging
[![GDB](https://img.shields.io/badge/GDB-Enabled-blue.svg)](https://github.com/Nanaimo2013/NansOS)
```bash
# Connect to QEMU
gdb
target remote localhost:1234
```

</td>
</tr>
</table>

## ❗ Common Issues

<table>
<tr>
<td width="50%">

### 🚫 Build Errors
[![Errors](https://img.shields.io/badge/Errors-Common-red.svg)](https://github.com/Nanaimo2013/NansOS)

1. **Size Errors**
   ```
   Stage 1 exceeds 512 bytes
   Stage 2 exceeds 1024 bytes
   ```
   - Optimize code
   - Move functionality

2. **Section Warnings**
   ```
   section below image base
   ```
   - Normal for bootloader
   - Can be ignored

</td>
<td width="50%">

### ⚠️ Runtime Issues
[![Issues](https://img.shields.io/badge/Issues-Runtime-orange.svg)](https://github.com/Nanaimo2013/NansOS)

1. **Boot Failures**
   - Check stage sizes
   - Verify load addresses
   - Check GDT setup

2. **Kernel Panics**
   - Use GDB debugging
   - Check memory map
   - Verify interrupts

</td>
</tr>
</table>

## 🔧 Build Customization

<table>
<tr>
<td width="50%">

### 🎛️ Custom Builds
[![Custom](https://img.shields.io/badge/Custom-Builds-blue.svg)](https://github.com/Nanaimo2013/NansOS)
```powershell
# Component build
.\build.ps1 -Component bootloader

# Custom version
.\build.ps1 -Version "1.1.0-alpha"
```

</td>
<td width="50%">

### 🌍 Environment
[![Env](https://img.shields.io/badge/Environment-Variables-green.svg)](https://github.com/Nanaimo2013/NansOS)
```powershell
$env:NANOS_DEBUG = 1
$env:NANOS_OPTIMIZE = 1
```

</td>
</tr>
</table>

## 🔄 Continuous Integration

<table>
<tr>
<td width="50%">

### 🔁 GitHub Actions
[![CI](https://img.shields.io/badge/CI-GitHub-blue.svg)](https://github.com/Nanaimo2013/NansOS)
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

</td>
<td width="50%">

### 📊 Build Matrix
[![Matrix](https://img.shields.io/badge/Matrix-Builds-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Windows/Linux
- Debug/Release
- x86/x64
- Test suites

</td>
</tr>
</table>

---

<div align="center">

**[🏠 Home](../README.md)** •
**[📖 Documentation](architecture.md)** •
**[🚀 Roadmap](roadmap.md)** •
**[💡 Contributing](../CONTRIBUTING.md)**

</div> 