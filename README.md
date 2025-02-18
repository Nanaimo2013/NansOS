# NansOS

<div align="center">

![NansOS Logo](docs/images/logo.png)

A RobCo-inspired 64-bit operating system with a retro-futuristic terminal interface.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/Nanaimo2013/NansOS/releases)

</div>

## Overview

NansOS is a bare metal operating system that recreates the iconic RobCo terminal experience from the Fallout series. Written in assembly and designed for modern 64-bit systems, it features a retro-futuristic interface while leveraging contemporary hardware capabilities.

### Features

- **64-bit Architecture**: Full support for modern x86_64 systems
- **RobCo-Style Interface**: Authentic green-text terminal experience
- **Hardware Support**:
  - PS/2 and USB keyboard/mouse input
  - VGA text and graphics modes
  - ATA/IDE disk operations
  - FAT12 filesystem
- **Modern Development**:
  - Cross-platform build system
  - Debugging support
  - Modular architecture

## Getting Started

### Prerequisites

- NASM (Netwide Assembler)
- MinGW-w64 (for Windows builds)
- QEMU (for testing)
- Git (for version control)

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/NansOS.git
   cd NansOS
   ```

2. Build the OS:
   ```bash
   # On Windows
   .\build.ps1
   # Or use the run script
   run.bat build
   ```

3. Run in QEMU:
   ```bash
   # On Windows
   run.bat
   ```

### Development

The project structure is organized as follows:
```
NansOS/
├── src/
│   ├── boot/          # Bootloader
│   ├── kernel/        # Kernel components
│   ├── drivers/       # Hardware drivers
│   ├── gui/           # Interface components
│   └── config/        # Build configuration
├── builds/            # Build outputs
├── docs/             # Documentation
└── tools/            # Development tools
```

## Documentation

- [Architecture Overview](docs/architecture.md)
- [Build System](docs/building.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Changelog](CHANGELOG.md)

## Screenshots

<div align="center">
<img src="docs/images/boot.png" width="400" alt="Boot Screen">
<img src="docs/images/terminal.png" width="400" alt="Terminal Interface">
</div>

## Contributing

Contributions are welcome! Please read our [Contributing Guide](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For available versions, see the [releases page](https://github.com/Nanaimo2013/NansOS/releases).

## Authors

- **Nanaimo_2013** - *Initial work* - [Nanaimo2013](https://github.com/Nanaimo2013)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by the RobCo terminals from the Fallout series
- Thanks to the OSDev community for their invaluable resources
- All contributors who help improve this project

## Status

- Current Version: 1.0.0
- Status: Active Development
- [View Roadmap](docs/roadmap.md)