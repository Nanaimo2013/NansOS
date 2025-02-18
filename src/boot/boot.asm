[BITS 16]           ; 16-bit Real Mode
[ORG 0x7C00]        ; BIOS loads bootloader at 0x7C00

; Constants
HARD_DISK    equ 0x80    ; First hard disk

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

    ; Load stage 2
    mov si, msg_stage2
    call print_string
    call load_stage2
    jc error

    ; Success message
    mov si, msg_ok
    call print_string

    ; Jump to stage 2
    mov dl, [bootDrive]    ; Pass boot drive to stage 2
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

; Load stage 2
load_stage2:
    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc .error

    ; Load stage 2 sectors
    mov ax, 0x07E0  ; Stage 2 segment
    mov es, ax
    xor bx, bx      ; Offset 0

    mov ah, 0x02    ; Read sectors
    mov al, 2       ; Number of sectors
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Start from sector 2
    mov dh, 0       ; Head 0
    mov dl, [bootDrive]
    int 0x13
    jc .error

    clc             ; Clear carry (success)
    ret

.error:
    stc             ; Set carry (error)
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