# NansOS Architecture Overview

<div align="center">

[![Architecture](https://img.shields.io/badge/Architecture-x86__64-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS)
[![Documentation](https://img.shields.io/badge/Documentation-Latest-green.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/docs)
[![Standard](https://img.shields.io/badge/Standard-System%20V%20ABI-orange.svg?style=for-the-badge)](https://wiki.osdev.org/System_V_ABI)

</div>

## 🏗️ System Architecture

<table>
<tr>
<td width="50%">

### 📦 Core Components
```
+------------------+
|    User Space    |
|  Applications    |
+------------------+
|   GUI System     |
| Window Manager   |
+------------------+
|  System Services |
| Device Drivers   |
+------------------+
|     Kernel      |
|  Memory, I/O    |
+------------------+
|   Bootloader    |
| Stage 1 & 2     |
+------------------+
```

</td>
<td width="50%">

### 🔑 Key Features
- **64-bit Architecture**
  - Long mode support
  - Extended memory access
  - Advanced paging
- **RobCo-Style Interface**
  - Terminal emulation
  - Green-text display
  - Command processing
- **Hardware Support**
  - PS/2 & USB devices
  - ATA/IDE storage
  - VGA display modes

</td>
</tr>
</table>

## 🚀 Boot Process

<table>
<tr>
<td width="25%">

### Stage 1
[![Size](https://img.shields.io/badge/Size-512B-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- BIOS loads MBR
- Basic setup
- Loads Stage 2

</td>
<td width="25%">

### Stage 2
[![Size](https://img.shields.io/badge/Size-1KB-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- Hardware detection
- Boot sequence
- Loads kernel

</td>
<td width="25%">

### Protected Mode
[![Mode](https://img.shields.io/badge/Mode-32bit-green.svg)](https://github.com/Nanaimo2013/NansOS)
- A20 line enabled
- GDT loaded
- Mode switch

</td>
<td width="25%">

### Long Mode
[![Mode](https://img.shields.io/badge/Mode-64bit-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Paging setup
- PAE enabled
- Final switch

</td>
</tr>
</table>

## 💾 Memory Management

### 🗺️ Memory Map
```
0x00000000 - 0x000003FF: Real Mode IVT
0x00000400 - 0x000004FF: BIOS Data Area
0x00000500 - 0x00007BFF: Free Memory
0x00007C00 - 0x00007DFF: Bootloader
0x00007E00 - 0x0009FFFF: Free Memory
0x000A0000 - 0x000BFFFF: Video Memory
0x000C0000 - 0x000FFFFF: BIOS ROM
0x00100000 - 0x00FFFFFF: Kernel Space
0x01000000 - .........: User Space
```

### 📑 Paging Structure
[![Paging](https://img.shields.io/badge/Paging-4--Level-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- 4-level paging hierarchy
- 4KB standard pages
- 2MB large pages supported
- Page-level protection

## 🖥️ Hardware Support

<table>
<tr>
<td width="33%">

### ⌨️ Input Devices
[![Status](https://img.shields.io/badge/Status-Supported-green.svg)](https://github.com/Nanaimo2013/NansOS)
- PS/2 Keyboard
- PS/2 Mouse
- USB HID (planned)

</td>
<td width="33%">

### 🖥️ Display
[![Status](https://img.shields.io/badge/Status-Active-green.svg)](https://github.com/Nanaimo2013/NansOS)
- VGA Text Mode
- Graphics Mode
- VESA Support

</td>
<td width="33%">

### 💽 Storage
[![Status](https://img.shields.io/badge/Status-Active-green.svg)](https://github.com/Nanaimo2013/NansOS)
- ATA/IDE Drives
- FAT12 Filesystem
- USB Storage

</td>
</tr>
</table>

## 🖼️ GUI System

<table>
<tr>
<td width="50%">

### 🪟 Window Manager
```
+----------------+
| Window Manager |
+----------------+
       |
  +---------+
  | Windows |
  +---------+
       |
+------------------+
| Terminal Emulator|
+------------------+
```

</td>
<td width="50%">

### 💻 Terminal Interface
[![Style](https://img.shields.io/badge/Style-RobCo-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Green-text display mode
- Command-line interface
- Multi-window support
- Custom font rendering

</td>
</tr>
</table>

## 📂 File System

<table>
<tr>
<td width="50%">

### 💾 FAT12 Implementation
[![Type](https://img.shields.io/badge/Type-FAT12-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- 512-byte sectors
- 1.44MB floppy support
- Basic file operations
- Directory structure

</td>
<td width="50%">

### 📁 Directory Layout
```
/
├── bin/     # System binaries
├── etc/     # Configuration
├── dev/     # Device files
└── usr/     # User files
```

</td>
</tr>
</table>

## 🔌 Device Driver Architecture

<table>
<tr>
<td width="60%">

### 🔧 Driver Model
```
+------------------+
|  Device Manager  |
+------------------+
         |
+------------------+
| Driver Interface |
+------------------+
         |
+------------------+
| Hardware Access  |
+------------------+
```

</td>
<td width="40%">

### 📝 Driver Categories
[![Status](https://img.shields.io/badge/Status-Active-green.svg)](https://github.com/Nanaimo2013/NansOS)
1. Character Devices
2. Block Devices
3. Network Devices

</td>
</tr>
</table>

## 🔒 Security Model

<table>
<tr>
<td width="33%">

### 💍 Protection Rings
[![Rings](https://img.shields.io/badge/Rings-4--Level-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- Ring 0: Kernel
- Ring 3: User

</td>
<td width="33%">

### 🛡️ Memory Protection
[![Protection](https://img.shields.io/badge/Protection-Active-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Page-level protection
- Segment isolation
- Resource control

</td>
<td width="33%">

### 🔐 Access Control
[![Access](https://img.shields.io/badge/Access-Managed-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Process isolation
- Resource limits
- Permission system

</td>
</tr>
</table>

## 🛠️ Development Tools

<table>
<tr>
<td width="33%">

### 🏗️ Build System
[![Build](https://img.shields.io/badge/Build-PowerShell-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- PowerShell scripts
- NASM assembly
- GNU toolchain

</td>
<td width="33%">

### 🐛 Debugging
[![Debug](https://img.shields.io/badge/Debug-GDB-orange.svg)](https://github.com/Nanaimo2013/NansOS)
- GDB support
- QEMU debugging
- Memory inspection

</td>
<td width="33%">

### 📊 Testing
[![Test](https://img.shields.io/badge/Test-Automated-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Unit testing
- Integration tests
- CI/CD pipeline

</td>
</tr>
</table>

---

<div align="center">

**[🏠 Home](../README.md)** •
**[📖 Documentation](building.md)** •
**[🚀 Roadmap](roadmap.md)** •
**[💡 Contributing](../CONTRIBUTING.md)**

</div>

## Future Architecture

### Planned Enhancements
1. **Network Stack**
   - TCP/IP support
   - Network services
   - Remote access

2. **Advanced Graphics**
   - Hardware acceleration
   - 3D effects
   - Custom rendering

3. **Audio System**
   - Sound driver
   - Multiple formats
   - Effect processing

## References
- [OSDev Wiki](https://wiki.osdev.org/)
- [Intel® 64 Architecture Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [System V ABI](https://wiki.osdev.org/System_V_ABI) 