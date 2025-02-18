[BITS 32]
; Kernel Entry Point for NansOS

section .text
global _start

_start:
    ; Set up stack
    mov esp, 0x90000

    ; Print welcome message
    mov esi, welcome_msg
    call print_string
    
    ; Halt
.halt:
    cli
    hlt
    jmp .halt

; Print string (ESI = string pointer)
print_string:
    push eax
    push ebx
    push ecx
    push edx
    
    mov ebx, 0xB8000    ; Video memory address
    mov ah, 0x0A        ; Light green on black
    
.loop:
    lodsb               ; Load next character
    test al, al        ; Check for null terminator
    jz .done
    
    mov [ebx], al      ; Store character
    mov [ebx+1], ah    ; Store attribute
    add ebx, 2         ; Next character position
    
    jmp .loop
.done:
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

section .data
align 4
welcome_msg:  db 'NansOS Kernel v1.0 loaded!', 0 