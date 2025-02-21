# NansOS Architecture Overview
By NansStudios

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
|    (Planned)     |
+------------------+
|   Basic Output   |
|    VGA Text      |
+------------------+
|  Basic Drivers   |
|    Hardware      |
+------------------+
|     Kernel      |
|  Memory, Boot   |
+------------------+
|   Bootloader    |
| Stage 1 & 2     |
+------------------+
```

</td>
<td width="50%">

### 🔑 Key Features
- **64-bit Architecture**
  - Protected mode
  - Long mode transition
  - Basic paging
- **Basic Interface**
  - VGA text mode
  - Color support
  - Simple output
- **Hardware Support**
  - Basic memory management
  - Simple hardware detection
  - Error handling

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
- Hardware check
- Mode preparation
- Loads kernel

</td>
<td width="25%">

### Protected Mode
[![Mode](https://img.shields.io/badge/Mode-32bit-green.svg)](https://github.com/Nanaimo2013/NansOS)
- A20 line
- GDT setup
- Mode switch

</td>
<td width="25%">

### Long Mode
[![Mode](https://img.shields.io/badge/Mode-64bit-green.svg)](https://github.com/Nanaimo2013/NansOS)
- Basic paging
- PAE setup
- Mode switch

</td>
</tr>
</table>

## 💾 Memory Management

### 🗺️ Memory Map
```
0x00000000 - 0x000003FF: Real Mode IVT
0x00000400 - 0x000004FF: BIOS Data Area
0x00000500 - 0x00007BFF: Free Memory
0x00007C00 - 0x00007DFF: Bootloader Stage 1
0x00007E00 - 0x00007FFF: Bootloader Stage 2
0x00008000 - 0x0009FFFF: Free Memory
0x000A0000 - 0x000BFFFF: Video Memory
0x000C0000 - 0x000FFFFF: BIOS ROM
0x00100000 - .........: Kernel Space
```

### 📑 Paging Structure
[![Paging](https://img.shields.io/badge/Paging-4--Level-blue.svg)](https://github.com/Nanaimo2013/NansOS)
- Basic 4-level paging
- 4KB pages
- Identity mapping
- Basic protection

## 🖥️ Hardware Support

<table>
<tr>
<td width="33%">

### ⌨️ Input/Output
[![Status](https://img.shields.io/badge/Status-Basic-yellow.svg)](https://github.com/Nanaimo2013/NansOS)
- VGA text mode
- Basic color support
- Simple output

</td>
<td width="33%">

### 🖥️ Display
[![Status](https://img.shields.io/badge/Status-Basic-yellow.svg)](https://github.com/Nanaimo2013/NansOS)
- Text mode only
- 80x25 display
- Color attributes

</td>
<td width="33%">

### 💽 Storage
[![Status](https://img.shields.io/badge/Status-Minimal-red.svg)](https://github.com/Nanaimo2013/NansOS)
- Basic disk read
- No filesystem yet
- Boot sector only

</td>
</tr>
</table>

## 🛠️ Development Tools

<table>
<tr>
<td width="33%">

### 🏗️ Build System
[![Build](https://img.shields.io/badge/Build-Basic-yellow.svg)](https://github.com/Nanaimo2013/NansOS)
- Make-based build
- NASM assembly
- GCC toolchain

</td>
<td width="33%">

### 🐛 Debugging
[![Debug](https://img.shields.io/badge/Debug-Basic-yellow.svg)](https://github.com/Nanaimo2013/NansOS)
- QEMU debugging
- Basic GDB support
- Error messages

</td>
<td width="33%">

### 📊 Testing
[![Test](https://img.shields.io/badge/Test-Planned-red.svg)](https://github.com/Nanaimo2013/NansOS)
- Manual testing
- QEMU-based tests
- Basic verification

</td>
</tr>
</table>

---

<div align="center">

**[🏠 Home](../README.md)** •
**[📖 Documentation](building.md)** •
**[🚀 Roadmap](roadmap.md)**

Made with ❤️ by NansStudios

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