# NansOS Architecture Overview

## System Architecture

### Core Components
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

## Boot Process

1. **Stage 1 Bootloader**
   - BIOS loads stage 1 from first sector (0x7C00)
   - Sets up basic environment
   - Initializes segment registers
   - Loads stage 2 into memory (0x7E00)
   - Transfers control to stage 2

2. **Stage 2 Bootloader**
   - Performs hardware detection
   - Shows RobCo-style boot sequence
   - Handles first boot vs quick boot
   - Loads kernel into memory (0x1000)
   - Transfers control to kernel

3. **Protected Mode Setup**
   - Enable A20 line
   - Load GDT
   - Switch to protected mode

4. **Long Mode Setup**
   - Set up paging
   - Enable PAE
   - Switch to long mode (64-bit)

5. **Kernel Initialization**
   - Initialize memory management
   - Set up interrupt handling
   - Start device drivers
   - Launch system services

## Memory Management

### Memory Map
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

### Paging Structure
- 4-level paging
- Page size: 4KB
- Support for large pages (2MB)

## Hardware Support

### Supported Devices
1. **Input Devices**
   - PS/2 Keyboard
   - PS/2 Mouse
   - USB HID (planned)

2. **Display**
   - VGA Text Mode
   - VGA Graphics Mode (planned)
   - VESA Support (planned)

3. **Storage**
   - ATA/IDE Drives
   - FAT12 Filesystem
   - USB Mass Storage (planned)

## GUI System

### Window Manager
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

### Terminal Interface
- RobCo-style display
- Green-text mode
- Command processing
- Multi-window support

## File System

### FAT12 Implementation
- 512-byte sectors
- 1.44MB floppy support
- Basic file operations

### Directory Structure
```
/
├── bin/     # System binaries
├── etc/     # Configuration
├── dev/     # Device files
└── usr/     # User files
```

## Device Driver Architecture

### Driver Model
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

### Driver Categories
1. Character Devices
2. Block Devices
3. Network Devices (planned)

## Security Model

### Protection Rings
- Ring 0: Kernel Mode
- Ring 3: User Mode

### Memory Protection
- Page-level protection
- Segment isolation
- Resource access control

## Development Tools

### Build System
- PowerShell-based build script
- NASM assembly
- GNU linker (LD)
- QEMU for testing

### Debugging
- GDB support
- QEMU debugging
- Memory inspection

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