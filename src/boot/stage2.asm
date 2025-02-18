[BITS 16]
[ORG 0x7E00]        ; Stage 2 loaded at 0x7E00

; Constants
KERNEL_LOAD_ADDR    equ 0x1000
STACK_ADDR          equ 0x90000

stage2_start:
    ; Set up segments
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive
    mov [bootDrive], dl

    ; Print welcome message
    mov si, msg_stage2
    call print_string

    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc error

    ; Load kernel
    mov si, msg_kernel
    call print_string
    call load_kernel
    jc error
    mov si, msg_ok
    call print_string

    ; Calculate GDT address
    mov ax, cs
    mov bx, gdt_start
    shl eax, 4
    add eax, ebx
    mov [gdt_descriptor + 2], eax

    ; Enter protected mode
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load GDT

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Switch to protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Flush pipeline with far jump
    jmp dword 0x08:protected_mode

[BITS 32]
protected_mode:
    ; Set up segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, STACK_ADDR

    ; Clear screen
    mov edi, 0xB8000
    mov ecx, 2000
    mov ax, 0x0720     ; White on black space
    rep stosw

    ; Jump to kernel
    mov eax, KERNEL_LOAD_ADDR
    jmp eax

error:
    mov si, msg_error
    call print_string
    jmp $

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

; Load kernel
load_kernel:
    ; Reset disk system
    mov ah, 0x00
    mov dl, [bootDrive]
    int 0x13
    jc .error

    ; Load kernel sectors
    mov ax, KERNEL_LOAD_ADDR >> 4  ; Convert to segment
    mov es, ax
    xor bx, bx      ; Offset 0

    ; Load first sector to check header
    mov ah, 0x02    ; Read sectors
    mov al, 1       ; One sector first
    mov ch, 0       ; Cylinder 0
    mov cl, 4       ; Start from sector 4 (after stage2)
    mov dh, 0       ; Head 0
    mov dl, [bootDrive]
    int 0x13
    jc .error

    ; Load rest of kernel
    mov ah, 0x02    ; Read sectors
    mov al, 40      ; Number of sectors (20KB)
    mov ch, 0       ; Cylinder 0
    mov cl, 5       ; Start from sector 5
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
msg_stage2:     db 'Stage 2 loaded', 0
msg_kernel:     db 'Loading kernel...', 0
msg_ok:         db '[OK]', 0
msg_error:      db 'Error: System halted', 0

; GDT
align 8
gdt_start:
    ; Null descriptor
    dq 0

    ; Code segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0000    ; Base (bits 0-15)
    db 0x00      ; Base (bits 16-23)
    db 10011010b ; Access (present, ring 0, code segment, executable, direction 0, readable)
    db 11001111b ; Granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)

    ; Data segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0000    ; Base (bits 0-15)
    db 0x00      ; Base (bits 16-23)
    db 10010010b ; Access (present, ring 0, data segment, executable, direction 0, writable)
    db 11001111b ; Granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd 0                        ; GDT address (filled at runtime) 