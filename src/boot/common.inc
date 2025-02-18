; Common routines for NansOS bootloader stages

; Print string routine (SI = string address)
print_string:
    pusha
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x0A    ; Light green on black
    int 0x10
    jmp .loop
.done:
    popa
    ret

; Print number in decimal (AX = number)
print_number:
    pusha
    mov cx, 0       ; Digit counter
    mov bx, 10      ; Divisor
.divide_loop:
    xor dx, dx      ; Clear DX for division
    div bx          ; Divide AX by 10
    push dx         ; Save remainder (digit)
    inc cx          ; Increment counter
    test ax, ax     ; Check if quotient is 0
    jnz .divide_loop
.print_loop:
    pop dx          ; Get digit
    add dl, '0'     ; Convert to ASCII
    mov ah, 0x0E
    mov al, dl
    int 0x10
    loop .print_loop
    popa
    ret

; Print progress bar
; Input: AL = percentage (0-100)
print_progress:
    pusha
    push ax
    
    ; Print percentage
    mov ah, 0x0E
    mov al, '['
    int 0x10
    
    pop ax          ; Restore percentage
    push ax
    
    ; Convert to number of bars (50 total)
    mov bl, 2
    div bl
    mov cl, al      ; Number of bars to print
    
    ; Print bars
    mov al, '='
.bar_loop:
    mov ah, 0x0E
    int 0x10
    dec cl
    jnz .bar_loop
    
    ; Fill remaining space
    pop ax          ; Restore percentage
    mov bl, 2
    div bl
    mov cl, 50
    sub cl, al      ; Remaining spaces
    mov al, ' '
.space_loop:
    mov ah, 0x0E
    int 0x10
    dec cl
    jnz .space_loop
    
    ; Print closing bracket and percentage
    mov al, ']'
    int 0x10
    mov al, ' '
    int 0x10
    
    popa
    ret

; Long delay routine (about 1 second)
delay_long:
    pusha
    mov cx, 0x0F
    mov dx, 0xFFFF
.loop:
    dec dx
    jnz .loop
    dec cx
    jnz .loop
    popa
    ret

; Short delay routine (about 0.1 second)
delay_short:
    pusha
    mov cx, 0x03
    mov dx, 0xFFFF
.loop:
    dec dx
    jnz .loop
    dec cx
    jnz .loop
    popa
    ret

; Print newline
print_newline:
    pusha
    mov ah, 0x0E
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    popa
    ret

; Print centered string
; SI = string address
print_centered:
    pusha
    ; First, measure string length
    mov cx, 0
    mov bx, si
.measure:
    mov al, [bx]
    test al, al
    jz .measure_done
    inc cx
    inc bx
    jmp .measure
    
.measure_done:
    ; Calculate padding (80 - length) / 2
    mov ax, 80
    sub ax, cx
    shr ax, 1      ; Divide by 2
    
    ; Print spaces
    mov cx, ax
.pad:
    mov ah, 0x0E
    mov al, ' '
    int 0x10
    loop .pad
    
    ; Print actual string
    call print_string
    
    popa
    ret

; Print header
; SI = header text
print_header:
    pusha
    call print_newline
    mov cx, 80
    mov ah, 0x0E
    mov al, '='
.top:
    int 0x10
    loop .top
    
    call print_newline
    call print_centered
    call print_newline
    
    mov cx, 80
    mov ah, 0x0E
    mov al, '='
.bottom:
    int 0x10
    loop .bottom
    call print_newline
    call print_newline
    
    popa
    ret 