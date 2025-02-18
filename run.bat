@echo off
setlocal EnableDelayedExpansion

:: Colors for output
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "CYAN=[96m"
set "NC=[0m"

:: Function to print status messages
:print_status
echo %GREEN%[*]%NC% %~1
goto :eof

:print_error
echo %RED%[!]%NC% %~1
goto :eof

:print_warning
echo %YELLOW%[!]%NC% %~1
goto :eof

:print_info
echo %CYAN%[i]%NC% %~1
goto :eof

:: Check for required tools
:check_dependencies
call :print_status "Checking dependencies..."

set "missing=0"
for %%i in (nasm.exe ld.exe qemu-system-x86_64.exe) do (
    where /q %%i
    if errorlevel 1 (
        call :print_error "Missing dependency: %%i"
        set /a "missing=1"
    )
)

if %missing% equ 1 (
    call :print_error "Please install missing dependencies and try again"
    call :print_info "Required tools:"
    echo   - NASM (Netwide Assembler) - https://www.nasm.us/
    echo   - MinGW-w64 (GNU toolchain) - https://www.mingw-w64.org/
    echo   - QEMU (System Emulator) - https://www.qemu.org/
    exit /b 1
)

call :print_status "All dependencies found"
goto :eof

:: Build the OS
:build_os
call :print_status "Building NansOS..."
powershell -ExecutionPolicy Bypass -File build.ps1
if errorlevel 1 (
    call :print_error "Build failed"
    exit /b 1
)
call :print_status "Build successful"
goto :eof

:: Run the OS
:run_os
call :print_status "Running NansOS in QEMU..."
call :print_info "Press Ctrl+Alt+G to release mouse, Ctrl+Alt+2 for monitor, Ctrl+Alt+X to exit"

if not exist "builds\nanos.img" (
    call :print_error "Disk image not found. Building first..."
    call :build_os
    if errorlevel 1 exit /b 1
)

:: Launch QEMU with proper configuration
start "NansOS" qemu-system-x86_64 ^
    -name "NansOS" ^
    -machine q35 ^
    -cpu qemu64 ^
    -m 128M ^
    -drive format=raw,file=builds/nanos.img,if=floppy ^
    -boot a ^
    -vga std ^
    -display sdl ^
    -monitor stdio ^
    -no-reboot ^
    -no-shutdown

if errorlevel 1 (
    call :print_error "QEMU failed to start"
    exit /b 1
)

call :print_info "QEMU started successfully. Use the monitor console for control."
goto :eof

:: Run the OS with debugging enabled
:debug_os
call :print_status "Running NansOS in debug mode..."
call :print_warning "Connect GDB to localhost:1234 to begin debugging"
call :print_info "GDB connection command: target remote localhost:1234"

if not exist "builds\nanos.img" (
    call :print_error "Disk image not found. Building first..."
    call :build_os
    if errorlevel 1 exit /b 1
)

:: Launch QEMU in debug mode
start "NansOS Debug" qemu-system-x86_64 ^
    -name "NansOS (Debug)" ^
    -machine q35 ^
    -cpu qemu64 ^
    -m 128M ^
    -drive format=raw,file=builds/nanos.img,if=floppy ^
    -boot a ^
    -vga std ^
    -display sdl ^
    -monitor stdio ^
    -no-reboot ^
    -no-shutdown ^
    -s -S

if errorlevel 1 (
    call :print_error "QEMU failed to start in debug mode"
    exit /b 1
)

call :print_info "QEMU started in debug mode. Waiting for GDB connection..."
goto :eof

:: Main script
:main
if "%1"=="clean" (
    call :print_status "Cleaning build artifacts..."
    powershell -ExecutionPolicy Bypass -File build.ps1 -Clean
) else if "%1"=="build" (
    call :check_dependencies
    call :build_os
) else if "%1"=="debug" (
    call :check_dependencies
    call :build_os
    call :debug_os
) else (
    call :check_dependencies
    call :build_os
    call :run_os
)
exit /b

:: Start execution
call :main %* 