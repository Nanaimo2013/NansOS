# NansOS Changelog

All notable changes to NansOS will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Enhanced terminal interface with command input support
- System check animation sequence
- OS component download simulation
- Progress percentage indicators
- Improved boot sequence visuals

### Changed
- Rebranded from RobCo to NanCo Industries
- Updated boot logo with ASCII art
- Enhanced memory test visualization
- Improved system status messages

### Fixed
- Boot sector formatting and alignment
- Memory test progress display
- System initialization sequence

## [1.0.0] - 2024-02-17
### Added
- Initial bootloader implementation
- NanCo-style boot sequence
- Memory test animation
- Progress bar animation
- Green text mode support
- Version tracking system

### Components
#### Bootloader v1.0.0
- Basic disk operations
- Memory testing
- Boot sequence animations
- Error handling
- BPB (BIOS Parameter Block) implementation

#### OS v1.0.0
- Initial release
- Basic system initialization
- Terminal interface foundation
- Memory management basics

### Technical Details
#### Boot Sequence
- Custom boot sector implementation
- FAT12 filesystem support
- Memory testing and system checks
- Component download simulation
- Interactive terminal interface

#### System Requirements
- x86 architecture
- 512KB minimum RAM
- Floppy disk or compatible storage
- BIOS with standard INT 13h support

### Known Issues
- Limited filesystem support
- Basic terminal functionality
- No persistent storage implementation

### Security Considerations
- Basic error handling
- No encryption
- Limited access controls 