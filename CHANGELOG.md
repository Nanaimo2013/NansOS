# NansOS Changelog
By NansStudios

All notable changes to NansOS will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Basic VGA text mode support
- Simple color text output
- Memory management foundations

### Changed
- Updated build system for x86_64
- Simplified project structure
- Basic bootloader implementation

### Fixed
- Boot sector formatting and alignment
- Memory initialization sequence
- Build system compatibility

## [0.1.0] - 2025-03-20
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