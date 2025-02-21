# NansOS Changelog
By NansStudios

All notable changes to NansOS will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Storage device driver interface
- Initial setup wizard
- Language selection support
- Display configuration options
- Keyboard layout selection
- Timezone configuration

### Changed
- Enhanced boot sequence with configuration options
- Improved keyboard and mouse driver initialization
- Updated system initialization messages
- Enhanced debug output formatting

### Fixed
- Keyboard initialization sequence
- Mouse driver detection
- Memory management initialization
- Serial port communication reliability

## [v0.2.0] - 2025-03-21
### Added
- Basic storage subsystem
- RAM disk implementation
- Virtual filesystem interface
- File operations support
- Device driver framework

### Changed
- Improved memory management
- Enhanced error handling
- Updated build system
- Better debug messaging

### Fixed
- Memory allocation bugs
- Interrupt handling issues
- Keyboard input processing
- System initialization sequence

### Security
- Added basic memory protection
- Improved error checking
- Enhanced system stability

## [v0.1.0] - 2025-03-20
### Added
- Initial bootloader implementation
- Basic 64-bit mode transition
- Memory paging setup
- Simple VGA text output
- Basic build system

### Components
#### Bootloader v0.1.0
- Basic disk operations
- Memory initialization
- Long mode transition
- Error handling

#### OS v0.1.0
- Initial release by NansStudios
- Basic system initialization
- Simple text output
- Basic memory management

### Technical Details
#### Boot Sequence
- Custom boot sector
- Protected mode transition
- Long mode setup
- Basic error handling

#### System Requirements
- x86_64 architecture
- 1MB minimum RAM
- QEMU for testing

### Known Issues
- Limited hardware support
- Basic text output only
- No filesystem implementation

### Security Considerations
- Basic error handling only
- No memory protection yet
- Limited system checks 