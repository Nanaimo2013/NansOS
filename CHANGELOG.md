# NansOS Changelog

All notable changes to NansOS will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-02-17

### Added
- Initial 64-bit kernel implementation
- RobCo-style terminal interface
- Hardware support:
  - PS/2 and USB keyboard drivers
  - Mouse support
  - VGA text mode and basic graphics
  - ATA/IDE disk operations
- File system:
  - FAT12 implementation
  - Basic file operations
- Memory Management:
  - 64-bit paging setup
  - Basic memory allocation
  - Memory mapping utilities
- Build System:
  - PowerShell-based build script
  - QEMU integration for testing
  - Debug support with GDB
- GUI Components:
  - Window management system
  - Desktop environment
  - File manager
  - System information display

### Technical Details
#### Boot Sequence
- Multi-stage bootloader
- Protected to long mode transition
- Memory detection and initialization
- Proper GDT and IDT setup
- Page table initialization

#### System Components
- Modular driver architecture
- Interrupt handling system
- Memory management system
- File system implementation

#### User Interface
- RobCo-inspired terminal interface
- Command-line interface
- Window management system
- File operations support

### Known Issues
- Limited USB device support
- Basic graphics mode only
- Limited file system operations

### Development Notes
- Build Requirements:
  - NASM (Netwide Assembler)
  - MinGW-w64 (GNU toolchain)
  - QEMU (for testing)
- Development Environment:
  - Windows/Linux compatible
  - PowerShell build system
  - Debugging tools integration

## Upcoming Features
- Enhanced graphics mode support
- Extended file system operations
- Network stack implementation
- Sound system support
- USB mass storage support
- System configuration interface

## References
- [Architecture Documentation](docs/architecture.md)
- [Build Instructions](docs/building.md)
- [Contributing Guidelines](CONTRIBUTING.md)

## Version History
### 1.x Series
- [v1.0.0](changelog/v1.0.0-CHANGELOG.md) - 2024-03-20
  - Initial release
  - 32-bit and 64-bit support
  - Basic driver implementation
  - Fallout-style interface

## Changelog Format
All changelogs follow our [template](changelog/CHANGELOG_TEMPLATE.md) and include:
- Build Information
- Added Features
- Changes
- Fixed Issues
- Technical Details
- Known Issues
- Development Notes
- References

## Contributing
When adding features or fixing bugs, please:
1. Create a new changelog entry if starting a new version
2. Use the changelog template
3. Update this index file
4. Include all relevant technical details

## Additional Information
- All dates are in YYYY-MM-DD format
- Version numbers follow [Semantic Versioning](https://semver.org/)
- Build numbers are incremental within each version 