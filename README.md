# NansOS

<div align="center">

<img src="docs/images/logo.png" alt="NansOS Logo" width="400px"/>

### A RobCo-inspired 64-bit operating system with a retro-futuristic terminal interface

<br/>

[![License](https://img.shields.io/badge/LICENSE-MIT-green.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/VERSION-1.0.0-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/releases)
[![Build](https://img.shields.io/badge/BUILD-passing-brightgreen.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/actions)

<br/>

[📖 Documentation](docs/architecture.md) •
[🚀 Getting Started](#-getting-started) •
[💡 Features](#-features) •
[📝 Contributing](CONTRIBUTING.md)

<br/>

<img src="docs/images/boot.png" width="400" alt="Boot Screen"/>
<img src="docs/images/terminal.png" width="400" alt="Terminal Interface"/>

</div>

<br/>

## ✨ Features

<table>
<tr>
<td>

### 🖥️ System Architecture
- 64-bit x86_64 support
- Protected & Long mode
- Modern memory management
- Hardware abstraction layer

</td>
<td>

### 🎮 Hardware Support
- PS/2 & USB input devices
- VGA text & graphics modes
- ATA/IDE disk operations
- FAT12 filesystem support

</td>
</tr>
<tr>
<td>

### 🛠️ Development Tools
- Cross-platform build system
- Integrated debugging
- QEMU testing support
- Modular architecture

</td>
<td>

### 🌟 RobCo Interface
- Authentic terminal experience
- Green-text display mode
- Command-line interface
- System monitoring tools

</td>
</tr>
</table>

## 🚀 Getting Started

### Prerequisites

<div align="center">

[![NASM 2.15+](https://img.shields.io/badge/NASM-2.15+-orange.svg?style=for-the-badge)](https://www.nasm.us/)
[![MinGW-w64](https://img.shields.io/badge/MinGW--w64-Latest-brightgreen.svg?style=for-the-badge)](http://mingw-w64.org/)
[![QEMU](https://img.shields.io/badge/QEMU-Latest-blue.svg?style=for-the-badge)](https://www.qemu.org/)
[![Git](https://img.shields.io/badge/Git-Latest-red.svg?style=for-the-badge)](https://git-scm.com/)

</div>

### Quick Start

```bash
# Clone repository
git clone https://github.com/Nanaimo2013/NansOS.git
cd NansOS

# Build OS
.\build.ps1    # Windows
./build.sh     # Linux

# Run in QEMU
run.bat        # Windows
./run.sh       # Linux
```

## 📦 Project Structure

```
NansOS/
├── src/               # Source code
│   ├── boot/         # Bootloader components
│   ├── kernel/       # Core kernel modules
│   ├── drivers/      # Hardware drivers
│   └── gui/          # Interface components
├── docs/             # Documentation
├── tools/            # Development tools
└── builds/           # Build artifacts
```

## 📊 Project Status

<div align="center">

[![Release](https://img.shields.io/badge/RELEASE-v1.0.0-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/releases)
[![Issues](https://img.shields.io/github/issues/Nanaimo2013/NansOS?style=for-the-badge&color=red)](https://github.com/Nanaimo2013/NansOS/issues)
[![PRs](https://img.shields.io/badge/PRs-welcome-yellow.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/pulls)

<br/>

[![Contributors](https://img.shields.io/github/contributors/Nanaimo2013/NansOS?style=for-the-badge&color=orange)](https://github.com/Nanaimo2013/NansOS/graphs/contributors)
[![Activity](https://img.shields.io/github/commit-activity/m/Nanaimo2013/NansOS?style=for-the-badge&color=blue)](https://github.com/Nanaimo2013/NansOS/commits/main)
[![Size](https://img.shields.io/github/repo-size/Nanaimo2013/NansOS?style=for-the-badge&color=purple)](https://github.com/Nanaimo2013/NansOS)

</div>

## 📚 Documentation

- [🏛️ Architecture Overview](docs/architecture.md)
- [🔨 Build System Guide](docs/building.md)
- [📝 Changelog](CHANGELOG.md)
- [🛣️ Project Roadmap](docs/roadmap.md)

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

<div align="center">

[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg?style=for-the-badge)](CODE_OF_CONDUCT.md)

</div>

## 📄 License

<div align="center">

[![MIT License](https://img.shields.io/badge/LICENSE-MIT-green.svg?style=for-the-badge)](LICENSE)

Copyright © 2024 NanCo Industries

</div>

## 🙏 Acknowledgments

- Inspired by RobCo terminals from Fallout
- Built with support from the OSDev community
- Special thanks to all contributors

---

<div align="center">

**[Website](https://nanaimo2013.github.io/NansOS)** •
**[Documentation](docs/architecture.md)** •
**[Report Bug](https://github.com/Nanaimo2013/NansOS/issues)** •
**[Request Feature](https://github.com/Nanaimo2013/NansOS/issues)**

</div>