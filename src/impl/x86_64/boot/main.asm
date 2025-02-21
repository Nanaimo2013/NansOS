;------------------------------------------------------------------------------
; NansOS Boot Loader
; Copyright (c) 2025 NansStudios
;
; This is the main boot loader for NansOS. It performs the following tasks:
; 1. Verifies multiboot compatibility
; 2. Checks CPU features
; 3. Sets up paging
; 4. Transitions to long mode
; 5. Jumps to 64-bit kernel
;------------------------------------------------------------------------------

global _start
global start
extern long_mode_start

section .text
bits 32
_start:
start:
    ; Save multiboot info
    mov edi, ebx    ; Multiboot info structure pointer
    mov esi, eax    ; Multiboot magic number

    ; Set up stack pointer
    mov esp, stack_top

    ; Perform system checks
    call check_multiboot
    call check_cpuid
    call check_long_mode

    ; Set up paging and enter long mode
    call set_up_page_tables
    call enable_paging

    ; Load GDT and jump to long mode
    lgdt [gdt64.pointer]
    jmp gdt64.code_segment:long_mode_start

;------------------------------------------------------------------------------
; System Check Functions
;------------------------------------------------------------------------------

check_multiboot:
    cmp esi, 0x36d76289    ; Compare saved magic number
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "M"
    jmp error

check_cpuid:
    ; Check if CPUID is supported by attempting to flip the ID bit (bit 21)
    ; in the FLAGS register
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "C"
    jmp error

check_long_mode:
    ; Test if extended processor info is available
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    ; Test if long mode is available
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode

    ret
.no_long_mode:
    mov al, "L"
    jmp error

;------------------------------------------------------------------------------
; Paging Setup
;------------------------------------------------------------------------------

set_up_page_tables:
    ; Map first P4 entry to P3 table
    mov eax, page_table_l3
    or eax, 0b11    ; present + writable
    mov [page_table_l4], eax

    ; Map first P3 entry to P2 table 1
    mov eax, page_table_l2
    or eax, 0b11    ; present + writable
    mov [page_table_l3], eax

    ; Map second P3 entry to P2 table 2 (for additional 1 GiB)
    mov eax, page_table_l2_2
    or eax, 0b11    ; present + writable
    mov [page_table_l3 + 8], eax  ; Next P3 entry

    ; Map each P2 entry to a huge 2MiB page (for first P2 table)
    mov ecx, 0         ; Counter variable
.map_p2_table1:
    mov eax, 0x200000  ; 2MiB
    mul ecx            ; Start address of each page
    or eax, 0b10000011 ; present + writable + huge
    mov [page_table_l2 + ecx * 8], eax

    inc ecx
    cmp ecx, 512       ; Check if whole P2 table is mapped
    jne .map_p2_table1 ; If not, continue mapping

    ; Map each P2 entry to a huge 2MiB page (for second P2 table)
    mov ecx, 0         ; Reset counter
.map_p2_table2:
    mov eax, 0x200000  ; 2MiB
    mul ecx
    add eax, 0x40000000  ; Start address for second P2 table (1 GiB offset)
    or eax, 0b10000011
    mov [page_table_l2_2 + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_p2_table2

    ret


enable_paging:
    ; Load P4 to cr3 register (CPU uses this to access the P4 table)
    mov eax, page_table_l4
    mov cr3, eax

    ; Enable PAE-flag in cr4 (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

;------------------------------------------------------------------------------
; Error Handler
;------------------------------------------------------------------------------

error:
    ; Print "ERR: X" where X is the error code
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

;------------------------------------------------------------------------------
; Data Sections
;------------------------------------------------------------------------------

section .bss
align 4096
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096
page_table_l2:
    resb 4096
page_table_l2_2:  ; Second P2 table for additional 1 GiB
    resb 4096
stack_bottom:
    resb 4096 * 4    ; 16 KiB stack
stack_top:

section .rodata
gdt64:
    dq 0    ; Zero entry
.code_segment: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; Code segment
.pointer:
    dw $ - gdt64 - 1    ; Length
    dq gdt64            ; Address
