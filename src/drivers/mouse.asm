[BITS 32]
; Mouse Driver for NansOS
; Supports both PS/2 and USB mice

; Mouse controller ports
MOUSE_DATA_PORT    equ 0x60
MOUSE_STATUS_PORT  equ 0x64
MOUSE_CMD_PORT     equ 0x64

; Mouse commands
MOUSE_RESET_CMD    equ 0xFF
MOUSE_ENABLE_CMD   equ 0xF4
MOUSE_DISABLE_CMD  equ 0xF5

section .data
    mouse_packet:     times 4 db 0   ; Mouse data packet
    mouse_cycle:      db 0           ; Current byte in packet
    mouse_x:          dd 0           ; Current X position
    mouse_y:          dd 0           ; Current Y position
    mouse_buttons:    db 0           ; Button states
    mouse_type:       db 0           ; 0 = PS/2, 1 = USB

section .text
global mouse_init
global mouse_handler
global mouse_get_position
global mouse_get_buttons

mouse_init:
    push ebp
    mov ebp, esp
    
    ; Enable auxiliary mouse device
    call mouse_wait_write
    mov al, 0xA8
    out MOUSE_CMD_PORT, al
    
    ; Enable interrupts
    call mouse_wait_write
    mov al, 0x20
    out MOUSE_CMD_PORT, al
    
    call mouse_wait_read
    in al, MOUSE_DATA_PORT
    or al, 3
    push eax
    
    call mouse_wait_write
    mov al, 0x60
    out MOUSE_CMD_PORT, al
    
    call mouse_wait_write
    pop eax
    out MOUSE_DATA_PORT, al
    
    ; Set mouse settings
    call mouse_write_cmd
    mov al, 0xF6        ; Set defaults
    call mouse_write_data
    
    call mouse_write_cmd
    mov al, MOUSE_ENABLE_CMD
    call mouse_write_data
    
    ; Reset mouse position
    mov dword [mouse_x], 0
    mov dword [mouse_y], 0
    mov byte [mouse_buttons], 0
    mov byte [mouse_cycle], 0
    
    mov esp, ebp
    pop ebp
    ret

mouse_handler:
    push ebp
    mov ebp, esp
    pushad
    
    ; Read mouse data
    in al, MOUSE_DATA_PORT
    
    ; Store in packet buffer
    movzx ebx, byte [mouse_cycle]
    mov [mouse_packet + ebx], al
    inc ebx
    and ebx, 3          ; Wrap around (4 bytes)
    mov [mouse_cycle], bl
    
    ; Process complete packet
    cmp bl, 3
    jne .done
    
    ; Update mouse state
    movzx eax, byte [mouse_packet]
    mov [mouse_buttons], al
    
    ; Update X position
    movsx eax, byte [mouse_packet + 1]
    add [mouse_x], eax
    
    ; Update Y position
    movsx eax, byte [mouse_packet + 2]
    neg eax             ; Y movement is inverted
    add [mouse_y], eax
    
    ; Clamp positions to screen boundaries
    mov eax, [mouse_x]
    cmp eax, 0
    jge .check_max_x
    mov dword [mouse_x], 0
    
.check_max_x:
    cmp eax, 639       ; Max X (assuming 640x480)
    jle .check_y
    mov dword [mouse_x], 639
    
.check_y:
    mov eax, [mouse_y]
    cmp eax, 0
    jge .check_max_y
    mov dword [mouse_y], 0
    
.check_max_y:
    cmp eax, 479       ; Max Y (assuming 640x480)
    jle .done
    mov dword [mouse_y], 479
    
.done:
    ; Send EOI to PIC
    mov al, 0x20
    out 0xA0, al       ; Slave PIC
    out 0x20, al       ; Master PIC
    
    popad
    mov esp, ebp
    pop ebp
    iret

mouse_get_position:
    push ebp
    mov ebp, esp
    
    ; Get pointer to X coordinate
    mov eax, [ebp + 8]
    mov edx, [mouse_x]
    mov [eax], edx
    
    ; Get pointer to Y coordinate
    mov eax, [ebp + 12]
    mov edx, [mouse_y]
    mov [eax], edx
    
    mov esp, ebp
    pop ebp
    ret

mouse_get_buttons:
    mov al, [mouse_buttons]
    ret

mouse_wait_read:
    in al, MOUSE_STATUS_PORT
    test al, 1
    jz mouse_wait_read
    ret

mouse_wait_write:
    in al, MOUSE_STATUS_PORT
    test al, 2
    jnz mouse_wait_write
    ret

mouse_write_cmd:
    call mouse_wait_write
    mov al, 0xD4
    out MOUSE_CMD_PORT, al
    ret

mouse_write_data:
    call mouse_wait_write
    out MOUSE_DATA_PORT, al
    
    call mouse_wait_read
    in al, MOUSE_DATA_PORT
    cmp al, 0xFA        ; Check for ACK
    jne mouse_write_data ; Retry if no ACK
    ret 