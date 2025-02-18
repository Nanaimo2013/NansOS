[BITS 16]
[ORG 0x7E00]        ; Stage 2 loaded at 0x7E00

; Constants
KERNEL_LOAD_ADDR    equ 0x1000     ; Load kernel at 0x1000
STACK_ADDR          equ 0x9000     ; Stack at 0x9000
KERNEL_SECTOR       equ 4          ; Kernel starts at sector 4
KERNEL_SEGMENTS     equ 40         ; Load 40 sectors (20KB)

stage2_start:
    ; Set up segments
    cli             ; Disable interrupts while setting up segments
    mov ax, 0       ; We want segment 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, STACK_ADDR
    sti             ; Re-enable interrupts

    ; Save boot drive
    mov [bootDrive], dl

    ; Clear screen
    mov ax, 0x0003  ; Text mode 80x25
    int 0x10

    ; Print welcome banner
    mov si, msg_banner
    call print_string
    mov si, msg_copyright
    call print_string
    mov si, msg_detect
    call print_string

    ; Detect available drives
    call detect_drives
    
    ; Load kernel installer
    mov si, msg_loading
    call print_string
    
    ; Try INT 13h extensions first
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [bootDrive]
    int 0x13
    jc load_kernel_legacy

    ; Load using extensions
    mov si, dap
    mov ah, 0x42
    mov dl, [bootDrive]
    int 0x13
    jnc kernel_loaded

load_kernel_legacy:
    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc disk_error
    
    ; Set up ES:BX for kernel load
    mov ax, KERNEL_LOAD_ADDR
    mov es, ax
    xor bx, bx
    
    ; Load kernel sectors
    mov ah, 0x02
    mov al, KERNEL_SEGMENTS
    mov ch, 0
    mov cl, KERNEL_SECTOR
    mov dh, 0
    mov dl, [bootDrive]
    int 0x13
    jc disk_error

kernel_loaded:
    mov si, msg_ok
    call print_string

    ; Pass drive info to kernel
    mov dl, [bootDrive]
    mov dh, [driveCount]
    
    ; Jump to kernel installer
    jmp KERNEL_LOAD_ADDR:0000

; Detect available drives
detect_drives:
    pusha
    mov byte [driveCount], 0
    
    ; Check for hard drives (80h-8Fh)
    mov dl, 0x80
.hdd_loop:
    mov ah, 0x08        ; Get drive parameters
    int 0x13
    jc .next_drive
    
    ; Valid drive found
    inc byte [driveCount]
    push dx
    
    ; Print drive info
    mov si, msg_drive
    call print_string
    
    pop dx
    push dx
    
    ; Print drive number
    mov al, dl
    call print_hex_byte
    
    ; Get drive size
    mov ah, 0x48
    mov si, drive_params
    int 0x13
    jc .skip_size
    
    ; Print drive size
    mov si, msg_size
    call print_string
    mov eax, [drive_params + drive_params_t.sectors]
    call print_dec
    mov si, msg_mb
    call print_string
    
.skip_size:
    pop dx
    
.next_drive:
    inc dl
    cmp dl, 0x8F
    jbe .hdd_loop
    
    popa
    ret

; Print functions
print_string:
    pusha
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x0A
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    popa
    ret

print_hex_byte:
    pusha
    mov cl, 4
    shr al, cl
    call .print_digit
    pop ax
    and al, 0x0F
    call .print_digit
    popa
    ret
.print_digit:
    add al, '0'
    cmp al, '9'
    jbe .print
    add al, 7
.print:
    mov ah, 0x0E
    int 0x10
    ret

print_dec:
    pusha
    mov ecx, 10
    mov ebx, 0
.div_loop:
    xor edx, edx
    div ecx
    push edx
    inc ebx
    test eax, eax
    jnz .div_loop
.print_loop:
    pop eax
    add al, '0'
    mov ah, 0x0E
    int 0x10
    dec ebx
    jnz .print_loop
    popa
    ret

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp error

error:
    mov si, msg_error
    call print_string
    jmp $

; Data
bootDrive:      db 0
driveCount:     db 0

; Drive Parameters Buffer
struc drive_params_t
    .size:      resw 1
    .flags:     resw 1
    .cylinders: resd 1
    .heads:     resd 1
    .sectors:   resd 1
    .total:     resq 1
    .bytes:     resw 1
endstruc

drive_params:
    istruc drive_params_t
        at drive_params_t.size,       dw 30
        at drive_params_t.flags,      dw 0
        at drive_params_t.cylinders,  dd 0
        at drive_params_t.heads,      dd 0
        at drive_params_t.sectors,    dd 0
        at drive_params_t.total,      dq 0
        at drive_params_t.bytes,      dw 512
    iend

; Disk Address Packet
dap:
    db 0x10
    db 0
    dw KERNEL_SEGMENTS
    dw 0
    dw KERNEL_LOAD_ADDR
    dq KERNEL_SECTOR

; Messages
msg_banner:     db '================================', 13, 10
                db '    NansOS Installation Setup    ', 13, 10
                db '================================', 0
msg_copyright:  db 'Copyright (c) 2024 NanCo Industries', 0
msg_detect:     db 'Detecting storage devices...', 0
msg_drive:      db 'Found drive 0x', 0
msg_size:       db ' Size: ', 0
msg_mb:         db ' MB', 0
msg_loading:    db 'Loading installation kernel...', 0
msg_ok:         db '[OK]', 0
msg_disk_error: db 'Disk read error!', 0
msg_error:      db 'System halted', 0