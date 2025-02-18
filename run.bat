@echo off
:: Root run script for NansOS
:: This script forwards all parameters to the main run script in tools/scripts

:: Create tools/scripts directory if it doesn't exist
if not exist "tools\scripts" mkdir tools\scripts

:: Check if the run script exists
if not exist "tools\scripts\run.bat" (
    echo Error: Run script not found at: tools\scripts\run.bat
    exit /b 1
)

:: Forward all arguments to the main run script
tools\scripts\run.bat %* 