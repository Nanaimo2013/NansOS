[BITS 16]
[ORG 0x7E00]        ; Stage 2 loaded at 0x7E00

; Constants
KERNEL_LOAD_ADDR    equ 0x1000
STACK_ADDR          equ 0x90000
KERNEL_SECTOR       equ 4     ; Kernel starts at sector 4

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
    mov si, dap
    mov ah, 0x42
    mov dl, [bootDrive]
    int 0x13
    jnc enter_protected_mode ; If successful, enter protected mode
    
    ; Fall back to legacy loading if extension load fails
load_kernel_legacy:
    mov ax, KERNEL_LOAD_ADDR >> 4
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, 40              ; Load 40 sectors (20KB)
    mov ch, 0               ; Cylinder 0
    mov cl, KERNEL_SECTOR   ; Start sector
    mov dh, 0               ; Head 0
    mov dl, [bootDrive]
    int 0x13
    jc error

enter_protected_mode:
    cli                     ; Disable interrupts
    
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Switch to protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Jump to 32-bit code
    jmp dword 0x08:protected_mode

return_to_real_mode:
    [BITS 32]
    ; Load real mode segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Disable protected mode
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax
    
    ; Far jump to real mode
    jmp word 0x0000:real_mode_return

error:
    mov si, msg_error
    call print_string
    jmp $

[BITS 32]
protected_mode:
    ; Set up protected mode segments
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

[BITS 16]
real_mode_return:
    ; Restore real mode segments
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7C00
    
    sti                ; Enable interrupts
    
    mov si, msg_real_mode
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
    dw 40        ; Number of sectors
    dw 0         ; Offset
    dw KERNEL_LOAD_ADDR >> 4  ; Segment
    dq KERNEL_SECTOR ; Starting LBA

; Messages
msg_stage2:     db 'Stage 2 loaded', 0
msg_error:      db 'Error: System halted', 0
msg_real_mode:  db 'Returned to real mode', 0

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