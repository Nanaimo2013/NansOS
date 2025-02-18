[BITS 16]
; Kernel Entry Point for NansOS
; Handles transition to 64-bit mode

section .text
global _start

_start:
    ; Save boot drive number in a register we won't use until 64-bit mode
    mov bl, dl

    ; Disable interrupts
    cli
    
    ; Set up initial stack
    mov ax, 0
    mov ss, ax
    mov sp, 0x7C00
    
    ; Load GDT using absolute addressing
    mov ax, cs
    mov ds, ax
    mov eax, gdt_descriptor
    lgdt [eax]
    
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Set up initial page tables
    xor eax, eax
    mov edi, 0x1000    ; PML4 at 0x1000
    mov cr3, edi       ; Set CR3 to PML4
    mov ecx, 4096
    rep stosd          ; Clear tables
    
    ; Set up page table entries using absolute addresses
    mov dword [0x1000], 0x2003  ; PML4[0] = PDPT
    mov dword [0x2000], 0x3003  ; PDPT[0] = PDT
    mov dword [0x3000], 0x4003  ; PDT[0] = PT
    
    ; Identity map first 2MB
    mov edi, 0x4000    ; PT
    xor ebx, ebx       ; Start address (preserving bl for boot drive)
    mov ecx, 512       ; 512 entries
.map_pt:
    mov edx, ebx
    or edx, 3          ; Present + Writable
    mov [edi], edx
    add ebx, 0x1000    ; Next page
    add edi, 8         ; Next entry
    dec ecx
    jnz .map_pt
    
    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5     ; Set PAE bit
    mov cr4, eax
    
    ; Enable long mode
    mov ecx, 0xC0000080  ; EFER MSR
    rdmsr
    or eax, 1 << 8     ; Set LME bit
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31    ; Set PG bit
    mov cr0, eax
    
    ; Load segments
    mov ax, 0x10       ; Data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Far jump to 64-bit code using absolute address
    jmp dword 0x08:long_mode_start

section .data
align 8
gdt_start:
    dq 0                ; Null descriptor

gdt_code:              ; Code segment descriptor
    dw 0xFFFF          ; Limit (bits 0-15)
    dw 0               ; Base (bits 0-15)
    db 0               ; Base (bits 16-23)
    db 10011010b       ; Access (present, ring 0, code segment, executable, direction 0, readable)
    db 10101111b       ; Granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
    db 0               ; Base (bits 24-31)

gdt_data:              ; Data segment descriptor
    dw 0xFFFF          ; Limit (bits 0-15)
    dw 0               ; Base (bits 0-15)
    db 0               ; Base (bits 16-23)
    db 10010010b       ; Access (present, ring 0, data segment, executable, direction 0, writable)
    db 10101111b       ; Granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
    db 0               ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

section .text
[BITS 64]
long_mode_start:
    ; Store boot drive number in memory now that we're in 64-bit mode
    mov byte [rel boot_drive], bl
    
    ; Set up 64-bit stack
    mov rsp, 0x200000    ; Set stack at 2MB
    
    ; Clear registers (except bl which has our boot drive number)
    xor rax, rax
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor rbp, rbp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15
    
    ; Now we can clear rbx after storing boot drive
    xor rbx, rbx
    
    ; Initialize kernel
    extern kernel_main
    call kernel_main
    
    ; Halt if kernel returns
.halt:
    cli
    hlt
    jmp .halt

section .data
boot_drive: db 0 