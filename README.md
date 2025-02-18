# NansOS

<div align="center">

![NansOS Logo](docs/images/logo.png)

A RobCo-inspired 64-bit operating system with a retro-futuristic terminal interface.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/Nanaimo2013/NansOS/releases)
[![Build Status](https://img.shields.io/github/workflow/status/Nanaimo2013/NansOS/build)](https://github.com/Nanaimo2013/NansOS/actions)
[![Issues](https://img.shields.io/github/issues/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS/issues)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/Nanaimo2013/NansOS/pulls)
[![Last Commit](https://img.shields.io/github/last-commit/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS/commits)
[![Stars](https://img.shields.io/github/stars/Nanaimo2013/NansOS?style=social)](https://github.com/Nanaimo2013/NansOS/stargazers)

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

[![NASM](https://img.shields.io/badge/NASM-2.15%2B-blue.svg)](https://www.nasm.us/)
[![MinGW-w64](https://img.shields.io/badge/MinGW--w64-Latest-green.svg)](http://mingw-w64.org/)
[![QEMU](https://img.shields.io/badge/QEMU-Latest-orange.svg)](https://www.qemu.org/)
[![Git](https://img.shields.io/badge/Git-Latest-red.svg)](https://git-scm.com/)

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/Nanaimo2013/NansOS.git
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

[![Documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg)](docs/architecture.md)
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

[![Contributors](https://img.shields.io/github/contributors/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS/graphs/contributors)
[![Commit Activity](https://img.shields.io/github/commit-activity/m/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS/commits/main)

Contributions are welcome! Please read our [Contributing Guide](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## Versioning

[![Releases](https://img.shields.io/github/v/release/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS/releases)

We use [SemVer](http://semver.org/) for versioning. For available versions, see the [releases page](https://github.com/Nanaimo2013/NansOS/releases).

## Authors

- **Nanaimo_2013** - *Initial work* - [Nanaimo2013](https://github.com/Nanaimo2013)

## License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by the RobCo terminals from the Fallout series
- Thanks to the OSDev community for their invaluable resources
- All contributors who help improve this project

## Status

[![Release Date](https://img.shields.io/github/release-date/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS/releases)
[![Code Size](https://img.shields.io/github/languages/code-size/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS)
[![Repo Size](https://img.shields.io/github/repo-size/Nanaimo2013/NansOS)](https://github.com/Nanaimo2013/NansOS)

- Current Version: 1.0.0
- Status: Active Development
- [View Roadmap](docs/roadmap.md)