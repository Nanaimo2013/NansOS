[BITS 64]
; System Information for NansOS

section .data
    cpu_vendor:      times 13 db 0
    cpu_brand:       times 49 db 0
    memory_size:     dq 0
    memory_free:     dq 0
    disk_size:       dq 0
    disk_free:       dq 0
    
    ; Info strings
    cpu_info_str:    db 'CPU: ', 0
    mem_info_str:    db 'MEM: ', 0
    disk_info_str:   db 'DSK: ', 0
    time_info_str:   db 'TIME: ', 0

section .text
global get_cpu_info
global get_memory_info
global get_disk_info
global get_time_info
global sysinfo_init

extern print_char
extern print_string

; Initialize system information
sysinfo_init:
    push rbp
    mov rbp, rsp
    
    ; Detect CPU
    call detect_cpu
    
    ; Detect memory
    call detect_memory
    
    ; Detect disk
    call detect_disk
    
    mov rsp, rbp
    pop rbp
    ret

; Get CPU information string
get_cpu_info:
    push rbp
    mov rbp, rsp
    
    ; Print CPU info string
    mov rsi, cpu_info_str
    call print_string
    
    ; Print CPU vendor
    mov rsi, cpu_vendor
    call print_string
    
    mov rsp, rbp
    pop rbp
    ret

; Get memory information string
get_memory_info:
    push rbp
    mov rbp, rsp
    
    ; Print memory info string
    mov rsi, mem_info_str
    call print_string
    
    ; Print memory size
    mov rax, [memory_size]
    call print_dec
    
    ; Print "MB"
    mov al, 'M'
    call print_char
    mov al, 'B'
    call print_char
    
    mov rsp, rbp
    pop rbp
    ret

; Get disk information string
get_disk_info:
    push rbp
    mov rbp, rsp
    
    ; Print disk info string
    mov rsi, disk_info_str
    call print_string
    
    ; Print disk size
    mov rax, [disk_size]
    call print_dec
    
    ; Print "MB"
    mov al, 'M'
    call print_char
    mov al, 'B'
    call print_char
    
    mov rsp, rbp
    pop rbp
    ret

; Get time information string
get_time_info:
    push rbp
    mov rbp, rsp
    
    ; Print time info string
    mov rsi, time_info_str
    call print_string
    
    ; Get system time
    mov ah, 0x02
    int 0x1A
    
    ; Print hours
    mov al, ch
    call print_hex_byte
    
    ; Print colon
    mov al, ':'
    call print_char
    
    ; Print minutes
    mov al, cl
    call print_hex_byte
    
    mov rsp, rbp
    pop rbp
    ret

; Detect CPU information
detect_cpu:
    push rbp
    mov rbp, rsp
    
    ; Get vendor string
    xor rax, rax
    cpuid
    mov [cpu_vendor], ebx
    mov [cpu_vendor + 4], edx
    mov [cpu_vendor + 8], ecx
    mov byte [cpu_vendor + 12], 0
    
    ; Get brand string
    mov rax, 0x80000002
    cpuid
    mov [cpu_brand], eax
    mov [cpu_brand + 4], ebx
    mov [cpu_brand + 8], ecx
    mov [cpu_brand + 12], edx
    
    mov rax, 0x80000003
    cpuid
    mov [cpu_brand + 16], eax
    mov [cpu_brand + 20], ebx
    mov [cpu_brand + 24], ecx
    mov [cpu_brand + 28], edx
    
    mov rax, 0x80000004
    cpuid
    mov [cpu_brand + 32], eax
    mov [cpu_brand + 36], ebx
    mov [cpu_brand + 40], ecx
    mov [cpu_brand + 44], edx
    mov byte [cpu_brand + 48], 0
    
    mov rsp, rbp
    pop rbp
    ret

; Detect memory size
detect_memory:
    push rbp
    mov rbp, rsp
    
    ; Use int 0x15, eax = 0xE820 for memory detection
    mov rax, 0xE820
    xor rbx, rbx
    mov rcx, 24
    mov rdx, 0x534D4150  ; 'SMAP'
    mov rdi, memory_size
    int 0x15
    
    ; Store total memory size
    mov rax, [rdi + 8]   ; Length of memory region
    mov [memory_size], rax
    
    mov rsp, rbp
    pop rbp
    ret

; Detect disk size
detect_disk:
    push rbp
    mov rbp, rsp
    
    ; Get disk parameters
    mov ah, 0x08
    mov dl, 0x80        ; First hard disk
    int 0x13
    jc .error
    
    ; Calculate size in MB
    mov al, ch          ; Maximum cylinder number
    mov ah, cl
    shr ah, 6
    inc ax              ; Add 1 (cylinders start at 0)
    
    and cl, 0x3F        ; Maximum sector number
    
    mul cl              ; ax = cylinders * sectors
    mul dh              ; ax = cylinders * sectors * heads
    
    ; Convert to MB (divide by 2048 = sectors per MB)
    shr ax, 11
    mov [disk_size], rax
    jmp .done
    
.error:
    mov qword [disk_size], 0
    
.done:
    mov rsp, rbp
    pop rbp
    ret

; Print decimal number in RAX
print_dec:
    push rbp
    mov rbp, rsp
    push rbx
    push rcx
    push rdx
    
    mov rcx, 10
    mov rbx, rsp        ; Use stack as buffer
    
.convert_loop:
    xor rdx, rdx
    div rcx             ; Divide by 10
    add dl, '0'         ; Convert to ASCII
    dec rbx
    mov [rbx], dl
    test rax, rax
    jnz .convert_loop
    
.print_loop:
    mov al, [rbx]
    call print_char
    inc rbx
    cmp rbx, rsp
    jb .print_loop
    
    pop rdx
    pop rcx
    pop rbx
    mov rsp, rbp
    pop rbp
    ret

; Print hex byte in AL
print_hex_byte:
    push rbp
    mov rbp, rsp
    
    push rax
    shr al, 4
    call print_hex_digit
    pop rax
    and al, 0x0F
    call print_hex_digit
    
    mov rsp, rbp
    pop rbp
    ret

; Print hex digit in AL (lower 4 bits)
print_hex_digit:
    and al, 0x0F
    add al, '0'
    cmp al, '9'
    jle .print
    add al, 7
.print:
    call print_char
    ret 