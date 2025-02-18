[BITS 32]
; Memory Management for NansOS

section .data
    heap_start:     dd 0x100000    ; Start at 1MB
    heap_end:       dd 0x200000    ; End at 2MB
    next_free:      dd 0x100000    ; Next free memory location
    
    ; Memory block header structure (16 bytes)
    ; dword size
    ; dword flags (bit 0 = used)
    ; dword reserved
    ; dword magic (0xDEADBEEF)

section .text
global malloc
global free
global mem_init

; Initialize memory management
mem_init:
    push ebp
    mov ebp, esp
    
    ; Initialize heap
    mov eax, [heap_start]
    mov [next_free], eax
    
    mov esp, ebp
    pop ebp
    ret

; Allocate memory
; Input: size in bytes (ebp + 8)
; Output: pointer to allocated memory or 0 if failed
malloc:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    
    ; Get requested size
    mov ecx, [ebp + 8]
    add ecx, 16         ; Add header size
    
    ; Align to 16 bytes
    add ecx, 15
    and ecx, ~15
    
    ; Check if enough space
    mov eax, [next_free]
    add eax, ecx
    cmp eax, [heap_end]
    ja .fail
    
    ; Set up block header
    mov edi, [next_free]
    mov [edi], ecx          ; Store size
    mov dword [edi + 4], 1  ; Mark as used
    mov dword [edi + 8], 0  ; Reserved
    mov dword [edi + 12], 0xDEADBEEF  ; Magic number
    
    ; Update next free pointer
    add [next_free], ecx
    
    ; Return pointer to usable memory
    lea eax, [edi + 16]
    jmp .done
    
.fail:
    xor eax, eax
    
.done:
    pop edi
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret

; Free memory
; Input: pointer to memory (ebp + 8)
free:
    push ebp
    mov ebp, esp
    
    ; Get block header
    mov eax, [ebp + 8]
    sub eax, 16
    
    ; Verify magic number
    cmp dword [eax + 12], 0xDEADBEEF
    jne .done
    
    ; Mark as free
    mov dword [eax + 4], 0
    
.done:
    mov esp, ebp
    pop ebp
    ret 