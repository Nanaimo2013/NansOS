# Contributing to NansOS

First off, thank you for considering contributing to NansOS! It's people like you that make NansOS such a great tool.

## Code of Conduct

This project and everyone participating in it is governed by the NansOS Code of Conduct. By participating, you are expected to uphold this code. Please report unacceptable behavior to [project email].

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the issue list as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible:

* Use a clear and descriptive title
* Describe the exact steps which reproduce the problem
* Provide specific examples to demonstrate the steps
* Describe the behavior you observed after following the steps
* Explain which behavior you expected to see instead and why
* Include screenshots and animated GIFs if possible
* Include your environment details (OS, tools versions)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please include:

* A clear and descriptive title
* A detailed description of the proposed functionality
* Any possible drawbacks or considerations
* If possible, a rough implementation approach

### Pull Requests

1. Fork the repo and create your branch from `master`
2. If you've added code that should be tested, add tests
3. If you've changed APIs, update the documentation
4. Ensure the test suite passes
5. Make sure your code follows the existing style
6. Issue that pull request!

## Development Process

1. **Setup Development Environment**
   ```powershell
   # Clone repository
   git clone https://github.com/Nanaimo2013/NansOS.git
   cd NansOS
   
   # Install dependencies
   # (See docs/building.md for detailed instructions)
   ```

2. **Create Feature Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make Changes**
   - Write your code
   - Follow coding standards
   - Add/update tests
   - Update documentation

4. **Test Your Changes**
   ```powershell
   # Build and test
   .\build.ps1
   .\run.bat
   ```

5. **Commit Your Changes**
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   ```

6. **Submit Pull Request**
   - Push to your fork
   - Submit a pull request
   - Wait for review

## Coding Standards

### Assembly Style Guide
```nasm
; File header with description
[BITS 64]
section .data
    ; Clear descriptive names
    counter:    dq 0
    message:    db 'Hello', 0

section .text
global my_function

; Function header with description
my_function:
    ; Preserve registers
    push rbp
    mov rbp, rsp
    
    ; Clear indentation
    mov rax, [counter]
    inc rax
    mov [counter], rax
    
    ; Restore registers
    mov rsp, rbp
    pop rbp
    ret
```

### Commit Message Format
```
type(scope): description

[optional body]

[optional footer]
```

Types:
- feat: New feature
- fix: Bug fix
- docs: Documentation
- style: Formatting
- refactor: Code restructuring
- test: Adding tests
- chore: Maintenance

### Documentation

1. **Code Comments**
   - Explain complex algorithms
   - Document assumptions
   - Clarify non-obvious code

2. **Function Headers**
   ```nasm
   ; Function: process_input
   ; Description: Processes keyboard input
   ; Parameters:
   ;   - AL: Scancode
   ; Returns:
   ;   - RAX: Processed character
   ```

3. **File Headers**
   ```nasm
   ; NansOS Keyboard Driver
   ; Handles PS/2 keyboard input
   ; Author: Your Name
   ; Date: YYYY-MM-DD
   ```

## Testing

### Running Tests
```powershell
# Build and test
.\build.ps1
.\run.bat

# Debug mode
.\run.bat debug
```

### Adding Tests
1. Create test file in appropriate directory
2. Follow test naming convention
3. Update test documentation
4. Verify test coverage

## Questions?

Feel free to contact the project maintainers if you have any questions or need clarification.

## License

By contributing, you agree that your contributions will be licensed under the MIT License. 