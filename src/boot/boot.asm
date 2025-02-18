[BITS 16]           ; 16-bit Real Mode
[ORG 0x7C00]        ; BIOS loads bootloader at 0x7C00

jmp short start     ; Jump over BPB
nop                 ; Pad to 3 bytes for BPB

; BIOS Parameter Block
bpb_oem:            db 'NANSOS10'  ; OEM identifier (8 bytes)
bpb_bytes_sector:   dw 512        ; Bytes per sector
bpb_sec_cluster:    db 1          ; Sectors per cluster
bpb_reserved_sec:   dw 1          ; Reserved sectors
bpb_num_fats:       db 2          ; Number of FATs
bpb_root_entries:   dw 224        ; Root directory entries
bpb_total_sectors:  dw 2880       ; Total sectors (2880 * 512 = 1.44MB)
bpb_media:          db 0xF0       ; Media descriptor (0xF0 = 3.5" floppy)
bpb_sectors_fat:    dw 9          ; Sectors per FAT
bpb_sectors_track:  dw 18         ; Sectors per track
bpb_heads:          dw 2          ; Number of heads
bpb_hidden_sectors: dd 0          ; Hidden sectors
bpb_large_sectors:  dd 0          ; Large sector count
bpb_drive_num:      db 0x00       ; Drive number
bpb_reserved:       db 0          ; Reserved
bpb_signature:      db 0x29       ; Extended boot signature
bpb_volume_id:      dd 0x12345678 ; Volume serial number
bpb_volume_label:   db 'NansOS v1.0'  ; Volume label (11 bytes)
bpb_file_system:    db 'FAT12   '  ; File system type (8 bytes)

; Constants
STAGE1_SECTOR   equ 1     ; Stage 1 starts at sector 1 (after MBR)
STAGE2_SECTOR   equ 2     ; Stage 2 starts at sector 2
KERNEL_SECTOR   equ 4     ; Kernel starts at sector 4

; Boot sector entry point
start:
    ; Initialize segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive
    mov [bootDrive], dl

    ; Set video mode (clear screen)
    mov ax, 0x0003  ; Text mode 80x25
    int 0x10

    ; Print welcome message
    mov si, msg_welcome
    call print_string

    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc error

    ; Load stage 2 (2 sectors)
    mov si, msg_stage2
    call print_string
    
    ; Read stage 2 into memory
    mov ah, 0x02        ; Read sectors
    mov al, 0x02        ; Number of sectors to read
    mov ch, 0x00        ; Cylinder 0
    mov cl, STAGE2_SECTOR ; Sector number
    mov dh, 0x00        ; Head 0
    mov dl, [bootDrive] ; Drive number
    mov bx, 0x7E00      ; Load to ES:BX (0x0000:0x7E00)
    int 0x13
    jc error

    ; Success message
    mov si, msg_ok
    call print_string

    ; Jump to stage 2
    mov dl, [bootDrive]    ; Pass boot drive
    jmp 0x0000:0x7E00     ; Far jump to stage 2

error:
    mov si, msg_error
    call print_string
    jmp $                  ; Infinite loop

; Print string (SI = string pointer)
print_string:
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x0A    ; Light green
.loop:
    lodsb           ; Load next character
    test al, al     ; Check for null terminator
    jz .done
    int 0x10        ; Print character
    jmp .loop
.done:
    mov al, 13      ; Carriage return
    int 0x10
    mov al, 10      ; Line feed
    int 0x10
    ret

; Data
bootDrive:      db 0
msg_welcome:    db 'NansOS Bootloader v1.0', 0
msg_stage2:     db 'Loading stage 2...', 0
msg_ok:         db '[OK]', 0
msg_error:      db 'Error: System halted', 0

; Boot sector padding
times 510-($-$$) db 0  ; Fill remaining space with zeros
dw 0xAA55              ; Boot signature 