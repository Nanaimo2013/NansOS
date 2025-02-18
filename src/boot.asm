[BITS 16]           ; 16-bit Real Mode
[ORG 0x7C00]        ; BIOS loads bootloader at 0x7C00

jmp short start     ; Jump over the disk description table
nop                 ; Pad with NOP for BPB

; Disk Description Table (BPB - BIOS Parameter Block)
OEMLabel            db "NANCO  "  ; 8 bytes
BytesPerSector      dw 512
SectorsPerCluster   db 1
ReservedSectors     dw 1
NumberOfFATs        db 2
RootEntries         dw 224
TotalSectors        dw 2880        ; 1.44 MB floppy
MediaDescriptor     db 0xF0        ; 3.5" floppy
SectorsPerFAT       dw 9
SectorsPerTrack     dw 18
NumberOfHeads       dw 2
HiddenSectors       dd 0
TotalSectorsBig     dd 0
DriveNumber         db 0           ; 0 = floppy, 0x80 = hard disk
Reserved            db 0
Signature           db 0x29
VolumeID           dd 0x12345678
VolumeLabel         db "NANSOS BOOT"  ; 11 bytes
FileSystem          db "FAT12   "     ; 8 bytes

start:
    ; Initialize segment registers
    cli             ; Disable interrupts
    xor ax, ax      ; Zero AX register
    mov ds, ax      ; Set DS to 0
    mov es, ax      ; Set ES to 0
    mov ss, ax      ; Set SS to 0
    mov sp, 0x7C00  ; Set stack pointer
    sti             ; Enable interrupts

    ; Save boot drive number
    mov [bootDrive], dl

    ; Set video mode (80x25 text mode)
    mov ax, 0x0003  ; Text mode 3
    int 0x10

    ; Set text color to green
    mov ax, 0x0600  ; Clear screen
    mov bh, 0x0A    ; Light green on black
    mov cx, 0x0000  ; Upper left corner
    mov dx, 0x184F  ; Lower right corner
    int 0x10

    ; Display boot sequence
    mov si, msg_nanco_logo
    call print_string

    ; Load kernel
    mov si, msg_loading
    call print_string
    call load_kernel
    jc kernel_error

    ; Transfer control to kernel
    mov dl, [bootDrive]    ; Boot drive number
    mov ax, [isFirstBoot]  ; First boot flag
    jmp 0x0000:0x1000     ; Jump to kernel

; Load kernel from disk
load_kernel:
    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc .disk_error

    ; Load kernel sectors
    mov ax, 0x0100      ; Segment to load kernel into
    mov es, ax
    xor bx, bx          ; Offset to load kernel into

    mov ah, 0x02        ; BIOS read sector function
    mov al, 8           ; Number of sectors to read
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2
    mov dh, 0           ; Head 0
    mov dl, [bootDrive] ; Drive number
    int 0x13
    jc .disk_error

    clc                 ; Clear carry flag (success)
    ret

.disk_error:
    stc                 ; Set carry flag (error)
    ret

kernel_error:
    mov si, msg_error
    call print_string
    jmp $

; Print string routine (SI = string address)
print_string:
    pusha
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x0A
    int 0x10
    jmp .loop
.done:
    popa
    ret

; Data section
bootDrive:      db 0
isFirstBoot:    db 1        ; 1 = first boot, 0 = subsequent boots

msg_nanco_logo:
    db 'NanCo Industries OS', 13, 10
    db 'Copyright 2024', 13, 10, 13, 10, 0

msg_loading:
    db 'Loading OS...', 13, 10, 0

msg_error:
    db 'Boot Error!', 13, 10, 0

; Boot sector padding
times 510-($-$$) db 0  ; Fill remaining space with zeros
dw 0xAA55              ; Boot signature 