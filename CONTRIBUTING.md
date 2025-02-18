# Contributing to NansOS

<div align="center">

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/pulls)
[![Contributors](https://img.shields.io/badge/Contributors-Active-blue.svg?style=for-the-badge)](https://github.com/Nanaimo2013/NansOS/graphs/contributors)
[![Discord](https://img.shields.io/badge/Discord-Join%20Chat-7289DA.svg?style=for-the-badge)](https://discord.gg/nanos)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

</div>

## 🌟 Welcome!

First off, thank you for considering contributing to NansOS! It's people like you that make NansOS such a great tool.

<table>
<tr>
<td width="50%">

### 🤝 Code of Conduct
[![Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

We follow the Contributor Covenant. By participating, you are expected to uphold this code. Please report unacceptable behavior.

</td>
<td width="50%">

### 📝 License
[![MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

By contributing, you agree that your contributions will be licensed under the MIT License.

</td>
</tr>
</table>

## 🚀 How Can I Contribute?

<table>
<tr>
<td width="33%">

### 🐛 Bug Reports
[![Bugs](https://img.shields.io/badge/Bugs-Report-red.svg)](https://github.com/Nanaimo2013/NansOS/issues/new?template=bug_report.md)

Report bugs to help us improve:
- Clear title and description
- Steps to reproduce
- Expected vs actual behavior
- Screenshots if applicable
- System information

</td>
<td width="33%">

### 💡 Feature Requests
[![Features](https://img.shields.io/badge/Features-Request-green.svg)](https://github.com/Nanaimo2013/NansOS/issues/new?template=feature_request.md)

Suggest new features:
- Clear description
- Use case explanation
- Implementation ideas
- Potential impacts
- Related features

</td>
<td width="33%">

### 🔄 Pull Requests
[![PRs](https://img.shields.io/badge/PRs-Welcome-brightgreen.svg)](https://github.com/Nanaimo2013/NansOS/pulls)

Submit code changes:
- Fork the repo
- Create feature branch
- Add/update tests
- Update documentation
- Submit PR

</td>
</tr>
</table>

## 🛠️ Development Process

<table>
<tr>
<td width="50%">

### 🔧 Setup Environment
[![Setup](https://img.shields.io/badge/Setup-Development-blue.svg)](https://github.com/Nanaimo2013/NansOS)

```powershell
# Clone repository
git clone https://github.com/Nanaimo2013/NansOS.git
cd NansOS

# Install dependencies
# See docs/building.md
```

</td>
<td width="50%">

### 🌿 Branch Workflow
[![Git](https://img.shields.io/badge/Git-Workflow-orange.svg)](https://github.com/Nanaimo2013/NansOS)

```bash
# Create feature branch
git checkout -b feature/your-feature

# Make changes
git add .
git commit -m "feat: description"

# Submit PR
git push origin feature/your-feature
```

</td>
</tr>
</table>

## 📋 Coding Standards

<table>
<tr>
<td width="50%">

### 🔍 Assembly Style Guide
[![Style](https://img.shields.io/badge/Style-Assembly-blue.svg)](https://github.com/Nanaimo2013/NansOS)

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

</td>
<td width="50%">

### 📝 Commit Message Format
[![Commits](https://img.shields.io/badge/Commits-Conventional-green.svg)](https://github.com/Nanaimo2013/NansOS)

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

</td>
</tr>
</table>

## 📚 Documentation

<table>
<tr>
<td width="50%">

### 💭 Code Comments
[![Comments](https://img.shields.io/badge/Comments-Required-blue.svg)](https://github.com/Nanaimo2013/NansOS)

```nasm
; Function: process_input
; Description: Processes keyboard input
; Parameters:
;   - AL: Scancode
; Returns:
;   - RAX: Processed character
```

</td>
<td width="50%">

### 📖 File Headers
[![Headers](https://img.shields.io/badge/Headers-Required-blue.svg)](https://github.com/Nanaimo2013/NansOS)

```nasm
; NansOS Keyboard Driver
; Handles PS/2 keyboard input
; Author: Your Name
; Date: YYYY-MM-DD
```

</td>
</tr>
</table>

## 🧪 Testing

<table>
<tr>
<td width="50%">

### ▶️ Running Tests
[![Tests](https://img.shields.io/badge/Tests-Required-red.svg)](https://github.com/Nanaimo2013/NansOS)

```powershell
# Build and test
.\build.ps1
.\run.bat

# Debug mode
.\run.bat debug
```

</td>
<td width="50%">

### ➕ Adding Tests
[![Coverage](https://img.shields.io/badge/Coverage-Expanding-green.svg)](https://github.com/Nanaimo2013/NansOS)

1. Create test file
2. Follow naming convention
3. Update documentation
4. Verify coverage

</td>
</tr>
</table>

## ❓ Questions?

<table>
<tr>
<td width="50%">

### 💬 Community
[![Chat](https://img.shields.io/badge/Chat-Discord-7289DA.svg)](https://discord.gg/nanos)

Join our Discord server for:
- Quick questions
- Community support
- Development discussion
- Feature requests

</td>
<td width="50%">

### 📋 Issue Tracker
[![Issues](https://img.shields.io/badge/Issues-GitHub-green.svg)](https://github.com/Nanaimo2013/NansOS/issues)

Use GitHub Issues for:
- Bug reports
- Feature requests
- Documentation issues
- Project planning

</td>
</tr>
</table>

---

<div align="center">

**[🏠 Home](README.md)** •
**[📖 Documentation](docs/architecture.md)** •
**[🚀 Roadmap](docs/roadmap.md)** •
**[💡 Issues](https://github.com/Nanaimo2013/NansOS/issues)**

</div> 