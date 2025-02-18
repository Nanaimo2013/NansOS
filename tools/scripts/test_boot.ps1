# Simple test script for NansOS bootloader
Write-Host "Testing NansOS Bootloader..." -ForegroundColor Yellow

# Test Stage 1 size
Write-Host "`nTesting Stage 1 size..." -ForegroundColor Cyan
$stage1File = "bin/stage1_test.bin"
nasm -f bin -i "src/boot" "src/boot/boot.asm" -o $stage1File 2>&1 | Out-Null

if (Test-Path $stage1File) {
    $size = (Get-Item $stage1File).Length
    Write-Host "Stage 1 size: $size bytes"
    if ($size -gt 512) {
        Write-Host "ERROR: Stage 1 exceeds 512 bytes!" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "ERROR: Failed to compile Stage 1" -ForegroundColor Red
    exit 1
}

# Test full bootloader
Write-Host "`nTesting full bootloader..." -ForegroundColor Cyan
$bootFile = "bin/boot.bin"
nasm -f bin -i "src/boot" "src/boot/boot.asm" -o $bootFile 2>&1 | Out-Null

if (Test-Path $bootFile) {
    $totalSize = (Get-Item $bootFile).Length
    $stage2Size = $totalSize - 512
    Write-Host "Total bootloader size: $totalSize bytes"
    Write-Host "Stage 2 size: $stage2Size bytes"
    
    if ($stage2Size -gt 2048) {
        Write-Host "ERROR: Stage 2 exceeds 2048 bytes!" -ForegroundColor Red
        exit 1
    }
    
    # Check boot signature
    $bytes = [System.IO.File]::ReadAllBytes($bootFile)
    $signature = [BitConverter]::ToString($bytes[510..511])
    if ($signature -ne "55-AA") {
        Write-Host "ERROR: Invalid boot signature: $signature (expected: 55-AA)" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "ERROR: Failed to compile bootloader" -ForegroundColor Red
    exit 1
}

# Test kernel
Write-Host "`nTesting kernel..." -ForegroundColor Cyan
if (Test-Path "bin/kernel.bin") {
    $kernelSize = (Get-Item "bin/kernel.bin").Length
    Write-Host "Kernel size: $kernelSize bytes"
    
    if ($kernelSize -gt 65536) {
        Write-Host "ERROR: Kernel exceeds maximum size (64KB)" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "ERROR: Kernel binary not found" -ForegroundColor Red
    exit 1
}

# Test QEMU execution
Write-Host "`nTesting QEMU execution..." -ForegroundColor Cyan
Write-Host "Running QEMU with debug output..."

# Create debug script
$debugScript = @'
qemu-system-x86_64 -fda "builds/nanos.img" -d int -D qemu_debug.log -no-reboot
'@

Set-Content -Path "test_qemu.ps1" -Value $debugScript
Start-Process powershell -ArgumentList "-File test_qemu.ps1" -NoNewWindow -Wait

if (Test-Path "qemu_debug.log") {
    Write-Host "`nQEMU Debug Log:" -ForegroundColor Cyan
    Get-Content "qemu_debug.log" | Select-Object -First 20
    Remove-Item "qemu_debug.log"
}

Remove-Item "test_qemu.ps1" -ErrorAction SilentlyContinue

Write-Host "`nAll tests completed!" -ForegroundColor Green 