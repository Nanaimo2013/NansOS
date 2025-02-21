@echo off
setlocal enabledelayedexpansion

REM Check for MSYS2 installation
where bash >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: MSYS2 bash not found in PATH
    echo Please install MSYS2 and ensure it's in your PATH
    exit /b 1
)

REM Check for QEMU installation
where qemu-system-x86_64 >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: QEMU not found in PATH
    echo Please install QEMU and ensure it's in your PATH
    exit /b 1
)

REM Create test output directory
if exist test_output rmdir /s /q test_output
mkdir test_output

REM Run the test script using bash
bash ./tests/external_test.sh
set TEST_EXIT=%ERRORLEVEL%

REM Clean up
if exist test_output rmdir /s /q test_output

exit /b %TEST_EXIT% 