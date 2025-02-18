[BITS 16]
[ORG 0x7E00]        ; Stage 2 loaded at 0x7E00

; Constants
KERNEL_LOAD_ADDR    equ 0x100000  ; Load kernel at 1MB
STACK_ADDR          equ 0x90000
KERNEL_SECTOR       equ 4         ; Kernel starts at sector 4
KERNEL_SEGMENTS     equ 40        ; Load 40 sectors (20KB)

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

    ; Try to load kernel using INT 13h extensions
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [bootDrive]
    int 0x13
    jc load_kernel_legacy    ; If extensions not supported, use legacy method

    ; Load kernel using extensions
    mov si, msg_loading
    call print_string
    
    mov si, dap
    mov ah, 0x42
    mov dl, [bootDrive]
    int 0x13
    jnc kernel_loaded       ; If successful, continue
    
    ; Fall back to legacy loading if extension load fails
load_kernel_legacy:
    mov si, msg_legacy
    call print_string
    
    ; Convert linear address to segment:offset
    mov ax, KERNEL_LOAD_ADDR >> 4
    mov es, ax
    xor bx, bx
    
    ; Load kernel sectors
    mov ah, 0x02
    mov al, KERNEL_SEGMENTS  ; Number of sectors
    mov ch, 0               ; Cylinder 0
    mov cl, KERNEL_SECTOR   ; Start sector
    mov dh, 0               ; Head 0
    mov dl, [bootDrive]
    int 0x13
    jc error

kernel_loaded:
    mov si, msg_ok
    call print_string
    
    ; Prepare for protected mode
    cli                     ; Disable interrupts
    
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT
    lgdt [gdt_descriptor]
    
    mov si, msg_pmode
    call print_string

    ; Switch to protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Flush CPU pipeline with far jump
    jmp dword 0x08:.protected_mode_start

[BITS 32]
.protected_mode_start:
    ; Set up segment registers for protected mode
    mov ax, 0x10           ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, STACK_ADDR    ; Set up new stack

    ; Clear screen to indicate successful mode switch
    mov edi, 0xB8000
    mov ecx, 2000
    mov ax, 0x0720
    rep stosw

    ; Jump to kernel
    jmp dword 0x08:KERNEL_LOAD_ADDR

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

; Data
bootDrive:      db 0

; Disk Address Packet for extended loading
dap:
    db 0x10      ; Size of packet
    db 0         ; Reserved
    dw KERNEL_SEGMENTS  ; Number of sectors
    dw 0         ; Offset
    dw KERNEL_LOAD_ADDR >> 4  ; Segment
    dq KERNEL_SECTOR ; Starting LBA

; Messages
msg_stage2:     db 'NansOS Stage 2 Bootloader v1.0', 0
msg_loading:    db 'Loading kernel using INT 13h extensions...', 0
msg_legacy:     db 'Loading kernel using legacy BIOS calls...', 0
msg_ok:         db '[OK]', 0
msg_error:      db 'Error: System halted', 0
msg_pmode:      db 'Entering protected mode...', 0

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
    dd gdt_start                 ; GDT address