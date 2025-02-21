# NansOS

<div align="center">

### A 64-bit operating system developed by NansStudios

<br/>

[![License](https://img.shields.io/badge/LICENSE-MIT-green.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/VERSION-0.1.0-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/releases)
[![Build](https://img.shields.io/badge/BUILD-in_progress-yellow.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/actions)
[![Docker](https://img.shields.io/badge/Docker-ghcr.io-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/pkgs/container/nansos)

<br/>

[📖 Documentation](docs/architecture.md) •
[🚀 Getting Started](#-getting-started) •
[💡 Features](#-features)

</div>

<br/>

## ✨ Features

<table>
<tr>
<td>

### 🖥️ System Architecture
- 64-bit x86_64 support
- Protected & Long mode
- Basic memory management
- Simple hardware abstraction

</td>
<td>

### 🎮 Hardware Support
- Basic VGA text mode
- Simple memory operations
- Initial boot sequence

</td>
</tr>
<tr>
<td>

### 🛠️ Development Tools
- Cross-platform build system
- QEMU testing support
- Basic debugging support

</td>
<td>

### 🌟 Interface
- Basic text output
- Simple color support
- Initial boot messages

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
make build-x86_64

# Run in QEMU
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
```

## 📦 Project Structure

```
NansOS/
├── src/               # Source code
│   ├── impl/         # Implementation
│   │   ├── x86_64/   # Architecture-specific code
│   │   └── kernel/   # Kernel code
│   └── intf/         # Interfaces
├── targets/          # Build targets
└── buildenv/         # Build environment
```

## 📊 Project Status

<div align="center">

[![Release](https://img.shields.io/badge/RELEASE-v0.1.0-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/releases)
[![Issues](https://img.shields.io/github/issues/Nanaimo2013/NansOS?style=for-the-badge&color=red)](https://github.com/Nanaimo2013/NansOS/issues)
[![PRs](https://img.shields.io/badge/PRs-welcome-yellow.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/pulls)

</div>

## 📚 Documentation

- [🏛️ Architecture Overview](docs/architecture.md)
- [🔨 Build System Guide](docs/building.md)
- [📝 Changelog](CHANGELOG.md)
- [🛣️ Project Roadmap](docs/roadmap.md)

## 📄 License

<div align="center">

[![MIT License](https://img.shields.io/badge/LICENSE-MIT-green.svg?style=for-the-badge)](LICENSE)

Copyright © 2025 NansStudios. All rights reserved.

</div>

## 🐳 Docker Support

You can use our pre-built Docker image for development:

```bash
# Pull the latest image
docker pull ghcr.io/nanaimo2013/nansos:latest

# Run the container
docker run --rm -it -v ${PWD}:/root/env ghcr.io/nanaimo2013/nansos:latest

# Or build locally
docker build buildenv -t nansos-buildenv
docker run --rm -it -v ${PWD}:/root/env nansos-buildenv
```

The Docker image includes all necessary development tools and dependencies.

---

<div align="center">

**[Documentation](docs/architecture.md)** •
**[Report Bug](https://github.com/Nanaimo2013/NansOS/issues)** •
**[Request Feature](https://github.com/Nanaimo2013/NansOS/issues)**

Made with ❤️ by NansStudios

</div>