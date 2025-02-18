# Clean USB Drive Script for NansOS
# This script helps prepare a USB drive for use with Rufus

# Require admin privileges
#Requires -RunAsAdministrator

function Show-DiskList {
    Write-Host "`nAvailable disks:" -ForegroundColor Cyan
    $disks = Get-Disk | Where-Object { $_.BusType -eq "USB" }
    
    if ($disks.Count -eq 0) {
        Write-Host "No USB drives found!" -ForegroundColor Red
        exit 1
    }
    
    foreach ($disk in $disks) {
        Write-Host "`nDisk $($disk.Number):" -ForegroundColor Yellow
        Write-Host "  Friendly Name: $($disk.FriendlyName)"
        Write-Host "  Size: $([math]::Round($disk.Size/1GB, 2)) GB"
        Write-Host "  Partition Style: $($disk.PartitionStyle)"
        
        $partitions = Get-Partition -DiskNumber $disk.Number -ErrorAction SilentlyContinue
        if ($partitions) {
            Write-Host "  Partitions:"
            foreach ($partition in $partitions) {
                Write-Host "    - Size: $([math]::Round($partition.Size/1GB, 2)) GB, Type: $($partition.Type)"
            }
        }
    }
}

function Clean-USBDrive {
    param (
        [Parameter(Mandatory=$true)]
        [int]$DiskNumber
    )
    
    # Verify it's a USB drive
    $disk = Get-Disk -Number $DiskNumber
    if ($disk.BusType -ne "USB") {
        Write-Host "Error: Selected disk is not a USB drive!" -ForegroundColor Red
        exit 1
    }
    
    # Show warning and confirmation
    Write-Host "`nWARNING!" -ForegroundColor Red
    Write-Host "You are about to clean disk $DiskNumber : $($disk.FriendlyName)" -ForegroundColor Yellow
    Write-Host "Size: $([math]::Round($disk.Size/1GB, 2)) GB" -ForegroundColor Yellow
    Write-Host "ALL DATA ON THIS DRIVE WILL BE PERMANENTLY ERASED!" -ForegroundColor Red
    
    $confirm = Read-Host "`nType 'YES' (in uppercase) to confirm"
    if ($confirm -ne "YES") {
        Write-Host "Operation cancelled." -ForegroundColor Yellow
        exit 0
    }
    
    try {
        # Clean the disk using diskpart
        $diskpartScript = @"
select disk $DiskNumber
clean
exit
"@
        $diskpartScript | diskpart
        
        Write-Host "`nDisk cleaned successfully!" -ForegroundColor Green
        Write-Host "You can now use Rufus to create the bootable USB drive." -ForegroundColor Green
        
    } catch {
        Write-Host "Error cleaning disk: $_" -ForegroundColor Red
        exit 1
    }
}

# Main script
Write-Host "NansOS USB Drive Preparation Tool" -ForegroundColor Cyan
Write-Host "=================================" -ForegroundColor Cyan

# Show available USB drives
Show-DiskList

# Get user input
$diskNumber = Read-Host "`nEnter the disk number of your USB drive (or 'q' to quit)"
if ($diskNumber -eq 'q') {
    exit 0
}

# Clean the selected drive
Clean-USBDrive -DiskNumber ([int]$diskNumber) 