@echo off
:: NansOS Run Script
:: Runs the operating system in QEMU with GRUB

:: Parse arguments
set DEBUG=0
set BUILD=0
set MEMORY=512
set ARGS=

:parse_args
if "%1"=="" goto done_args
if /i "%1"=="debug" set DEBUG=1
if /i "%1"=="build" set BUILD=1
if /i "%1"=="memory" set MEMORY=%2 & shift
set ARGS=%ARGS% %1
shift
goto parse_args
:done_args

:: Build if requested
if %BUILD%==1 (
    echo Building NansOS...
    powershell -ExecutionPolicy Bypass -File build.ps1
    if errorlevel 1 (
        echo Build failed!
        exit /b 1
    )
)

echo Starting QEMU...

:: Set up QEMU command
set QEMU_CMD=qemu-system-i386 -kernel builds/boot/kernel.elf -monitor stdio

:: Add debug options if requested
if %DEBUG%==1 (
    echo Debug mode enabled. Connect with GDB using:
    echo   target remote localhost:1234
    set QEMU_CMD=%QEMU_CMD% -s -S
)

:: Run QEMU
%QEMU_CMD%

:: Check for errors
if errorlevel 1 (
    echo QEMU failed to start!
    echo Please ensure QEMU is installed and in your PATH
    exit /b 1
)

exit /b 0 