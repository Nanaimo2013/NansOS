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
STAGE2_LOAD_SEG    equ 0x07E0    ; Stage 2 load segment (0x7E00/16)
STAGE2_LOAD_OFF    equ 0x0000    ; Stage 2 load offset
STAGE2_SECTOR      equ 2         ; Stage 2 starts at sector 2

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

    ; Check for INT 13h extensions
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [bootDrive]
    int 0x13
    jc .no_extensions
    
    cmp bx, 0xAA55
    jne .no_extensions
    
    ; Extensions supported
    mov byte [use_extensions], 1
    mov si, msg_ext_support
    call print_string
    jmp .load_stage2
    
.no_extensions:
    mov si, msg_legacy
    call print_string

.load_stage2:
    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc error

    ; Load stage 2
    mov si, msg_stage2
    call print_string
    
    ; Check if using extensions
    cmp byte [use_extensions], 1
    je .load_ext
    
    ; Legacy loading
    mov ax, STAGE2_LOAD_SEG  ; Set up ES:BX
    mov es, ax
    xor bx, bx
    
    mov ah, 0x02            ; Read sectors
    mov al, 2               ; Number of sectors
    mov ch, 0               ; Cylinder 0
    mov cl, STAGE2_SECTOR   ; Sector number
    mov dh, 0               ; Head 0
    mov dl, [bootDrive]     ; Drive number
    int 0x13
    jc error
    jmp .load_done
    
.load_ext:
    ; Load using INT 13h extensions
    mov si, dap
    mov ah, 0x42
    mov dl, [bootDrive]
    int 0x13
    jc error

.load_done:
    ; Success message
    mov si, msg_ok
    call print_string

    ; Jump to stage 2
    mov dl, [bootDrive]     ; Pass boot drive
    jmp STAGE2_LOAD_SEG:STAGE2_LOAD_OFF

error:
    mov si, msg_error
    call print_string
    jmp $                   ; Infinite loop

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
use_extensions: db 0        ; 0 = legacy mode, 1 = use extensions

; Disk Address Packet
dap:
    db 0x10          ; Size of packet
    db 0             ; Reserved
    dw 2             ; Number of sectors
    dw 0             ; Offset
    dw STAGE2_LOAD_SEG ; Segment
    dq STAGE2_SECTOR ; Starting LBA

; Messages
msg_welcome:    db 'NansOS Bootloader v1.0', 0
msg_stage2:     db 'Loading stage 2...', 0
msg_ok:         db '[OK]', 0
msg_error:      db 'Error: System halted', 0
msg_ext_support: db 'INT 13h extensions supported', 0
msg_legacy:     db 'Using legacy BIOS calls', 0

; Boot sector padding
times 510-($-$$) db 0  ; Fill remaining space with zeros
dw 0xAA55              ; Boot signature 