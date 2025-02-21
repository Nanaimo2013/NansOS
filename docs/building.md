# NansOS Build System Documentation
By NansStudios

<div align="center">

[![Build Status](https://img.shields.io/badge/Build-In_Progress-yellow.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/actions)
[![Make](https://img.shields.io/badge/Make-4.0+-blue.svg?style=for-the-badge)](https://www.gnu.org/software/make/)
[![NASM](https://img.shields.io/badge/NASM-2.15+-orange.svg?style=for-the-badge)](https://www.nasm.us/)
[![GCC](https://img.shields.io/badge/GCC-x86__64--elf-green.svg?style=for-the-badge)](https://gcc.gnu.org/)

</div>

## 🚀 Quick Start

```bash
# Clone repository
git clone https://github.com/Nanaimo2013/NansOS.git
cd NansOS

# Build OS
make build-x86_64

# Run in QEMU
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
```

## 📋 Prerequisites

<table>
<tr>
<td width="50%">

### 🛠️ Required Tools
[![Tools](https://img.shields.io/badge/Tools-Required-red.svg)](https://github.com/Nanaimo2013/NansOS)

- NASM v2.15+
- GCC x86_64-elf cross-compiler
- GNU Make 4.0+
- QEMU (for testing)
- Docker (optional, for build environment)
- Git 2.0+
- CMake 3.10+ (for some components)

</td>
<td width="50%">

### 📥 Installation

#### Windows (Using Chocolatey)
```powershell
choco install make nasm qemu docker-desktop cmake git
```

#### Linux (Debian/Ubuntu)
```bash
apt install build-essential nasm qemu-system-x86 docker.io cmake git
```

</td>
</tr>
</table>

## 📁 Project Structure

<table>
<tr>
<td width="50%">

### 📂 Source Layout
```
src/
├── impl/           # Implementations
│   ├── x86_64/    # x86_64 specific
│   │   ├── boot/  # Boot code
│   │   └── ...    # Other arch code
│   ├── drivers/   # Device drivers
│   │   ├── storage/  # Storage drivers
│   │   ├── keyboard/ # Input drivers
│   │   └── ...    # Other drivers
│   └── kernel/    # Kernel code
└── intf/          # Interfaces
    └── ...        # Header files
```

</td>
<td width="50%">

### 🎯 Build Outputs
```
build/              # Build artifacts
├── x86_64/        # Architecture builds
│   ├── boot/      # Boot objects
│   ├── drivers/   # Driver objects
│   └── kernel/    # Kernel objects
└── ...            # Other artifacts

dist/               # Final outputs
└── x86_64/        # Distribution files
    ├── kernel.bin # Kernel binary
    └── kernel.iso # Bootable image
```

</td>
</tr>
</table>

## 🏗️ Build Process

### 📜 Build Targets

<table>
<tr>
<td width="50%">

#### Main Targets
```makefile
# Build everything
make all

# Build x86_64
make build-x86_64

# Clean build files
make clean

# Run tests
make test
```

</td>
<td width="50%">

#### Development Targets
```makefile
# Run in QEMU
make run

# Debug with GDB
make debug

# Build Docker env
make buildenv

# Run external tests
make external-test
```

</td>
</tr>
</table>

## ⚙️ Build Configuration

<table>
<tr>
<td width="33%">

### 🔧 Compiler Flags
```makefile
CFLAGS = -ffreestanding \
         -mno-red-zone \
         -mno-mmx \
         -mno-sse \
         -O2 \
         -Wall \
         -Wextra
```

</td>
<td width="33%">

### 🔨 Assembler Flags
```makefile
NASMFLAGS = -f elf64 \
            -F dwarf \
            -g \
            -w+all
```

</td>
<td width="33%">

### 🔗 Linker Flags
```makefile
LDFLAGS = -n \
          -T linker.ld \
          --no-dynamic-linker \
          -z max-page-size=0x1000
```

</td>
</tr>
</table>

## 🐳 Docker Build Environment

<table>
<tr>
<td width="50%">

### 🏗️ Building the Environment
```bash
# Build Docker image
docker build buildenv -t nansos-buildenv

# Run container
docker run --rm -it -v ${PWD}:/root/env nansos-buildenv

# Inside container
make build-x86_64
```

</td>
<td width="50%">

### 📝 Environment Details
- Based on Debian
- Pre-installed tools
- Cross-compiler setup
- Build dependencies
- Consistent environment
- Automated testing support

</td>
</tr>
</table>

## �� Testing

<table>
<tr>
<td width="50%">

### 🖥️ QEMU Testing
```bash
# Basic run
make run

# With serial output
make run-serial

# With monitor
make run-monitor

# With storage devices
make run-storage
```

</td>
<td width="50%">

### 🐛 Debugging
```bash
# Start GDB server
make debug

# In another terminal
gdb
(gdb) target remote localhost:1234
(gdb) symbol-file build/kernel.sym
(gdb) break kernel_main
```

</td>
</tr>
</table>

## ❗ Common Issues

<table>
<tr>
<td width="50%">

### 🚫 Build Errors
1. **Missing Tools**
   ```
   Command 'nasm' not found
   ```
   - Install required tools
   - Check PATH variable

2. **Permission Issues**
   ```
   Permission denied: dist/
   ```
   - Check directory permissions
   - Run with proper privileges

3. **Compilation Errors**
   ```
   undefined reference to...
   ```
   - Check include paths
   - Verify dependencies
   - Update toolchain

</td>
<td width="50%">

### ⚠️ Runtime Issues
1. **QEMU Problems**
   ```
   Could not initialize SDL
   ```
   - Install SDL libraries
   - Try different display backend

2. **GDB Connection**
   ```
   Connection refused
   ```
   - Check if QEMU is running
   - Verify port number

3. **Storage Issues**
   ```
   No boot device found
   ```
   - Check storage configuration
   - Verify device paths
   - Update QEMU parameters

</td>
</tr>
</table>

## 🔄 Continuous Integration

<table>
<tr>
<td width="50%">

### 👷 Build Pipeline
```yaml
name: Build
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: make build-x86_64
      - name: Test
        run: make external-test
```

</td>
<td width="50%">

### 🎯 Build Matrix
- Operating Systems
  - Ubuntu Latest
  - Windows Latest
- Architectures
  - x86_64
- Build Types
  - Debug
  - Release
- Test Suites
  - Unit Tests
  - Integration Tests
  - External Tests

</td>
</tr>
</table>

---

<div align="center">

**[🏠 Home](../README.md)** •
**[📖 Documentation](architecture.md)** •
**[🚀 Roadmap](roadmap.md)**

Made with ❤️ by NansStudios

</div> 