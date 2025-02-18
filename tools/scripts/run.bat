@echo off
setlocal enabledelayedexpansion

:: NansOS Run Script
:: This script handles running the OS in QEMU

:: Set colors for output
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set RESET=[0m

:: Check for QEMU
where qemu-system-x86_64 >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo %RED%Error: QEMU not found in PATH%RESET%
    echo Please install QEMU and add it to your PATH
    exit /b 1
)

:: Parse arguments
set BUILD=0
set DEBUG=0
set CLEAN=0

:parse_args
if "%1"=="" goto done_args
if /i "%1"=="build" set BUILD=1
if /i "%1"=="debug" set DEBUG=1
if /i "%1"=="clean" set CLEAN=1
shift
goto parse_args
:done_args

:: Clean if requested
if %CLEAN%==1 (
    echo %YELLOW%Cleaning build artifacts...%RESET%
    if exist "builds\nanos.img" del "builds\nanos.img"
    if exist "bin\*" del /q "bin\*"
)

:: Build if requested or if image doesn't exist
if %BUILD%==1 (
    echo %YELLOW%Building NansOS...%RESET%
    call powershell -ExecutionPolicy Bypass -File build.ps1
    if %ERRORLEVEL% neq 0 (
        echo %RED%Build failed!%RESET%
        exit /b 1
    )
)

if not exist "builds\nanos.img" (
    echo %YELLOW%No disk image found, building...%RESET%
    call powershell -ExecutionPolicy Bypass -File build.ps1
    if %ERRORLEVEL% neq 0 (
        echo %RED%Build failed!%RESET%
        exit /b 1
    )
)

:: Run QEMU
echo %GREEN%Starting NansOS...%RESET%

if %DEBUG%==1 (
    echo %YELLOW%Starting in debug mode%RESET%
    echo Connect GDB to localhost:1234 to begin debugging
    start qemu-system-x86_64 -fda "builds\nanos.img" -s -S
) else (
    qemu-system-x86_64 -fda "builds\nanos.img"
)

exit /b 0 