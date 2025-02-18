[BITS 32]
; System Information for NansOS

section .data
    cpu_vendor:      times 13 db 0
    cpu_brand:       times 49 db 0
    memory_size:     dd 0
    memory_free:     dd 0
    disk_size:       dd 0
    disk_free:       dd 0
    
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
    push ebp
    mov ebp, esp
    
    ; Detect CPU
    call detect_cpu
    
    ; Detect memory
    call detect_memory
    
    ; Detect disk
    call detect_disk
    
    mov esp, ebp
    pop ebp
    ret

; Get CPU information string
get_cpu_info:
    push ebp
    mov ebp, esp
    
    ; Print CPU info string
    mov esi, cpu_info_str
    call print_string
    
    ; Print CPU vendor
    mov esi, cpu_vendor
    call print_string
    
    mov esp, ebp
    pop ebp
    ret

; Get memory information string
get_memory_info:
    push ebp
    mov ebp, esp
    
    ; Print memory info string
    mov esi, mem_info_str
    call print_string
    
    ; Print memory size
    mov eax, [memory_size]
    call print_dec
    
    ; Print "MB"
    mov al, 'M'
    call print_char
    mov al, 'B'
    call print_char
    
    mov esp, ebp
    pop ebp
    ret

; Get disk information string
get_disk_info:
    push ebp
    mov ebp, esp
    
    ; Print disk info string
    mov esi, disk_info_str
    call print_string
    
    ; Print disk size
    mov eax, [disk_size]
    call print_dec
    
    ; Print "MB"
    mov al, 'M'
    call print_char
    mov al, 'B'
    call print_char
    
    mov esp, ebp
    pop ebp
    ret

; Get time information string
get_time_info:
    push ebp
    mov ebp, esp
    
    ; Print time info string
    mov esi, time_info_str
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
    
    mov esp, ebp
    pop ebp
    ret

; Detect CPU information
detect_cpu:
    push ebp
    mov ebp, esp
    
    ; Get vendor string
    xor eax, eax
    cpuid
    mov [cpu_vendor], ebx
    mov [cpu_vendor + 4], edx
    mov [cpu_vendor + 8], ecx
    mov byte [cpu_vendor + 12], 0
    
    ; Get brand string
    mov eax, 0x80000002
    cpuid
    mov [cpu_brand], eax
    mov [cpu_brand + 4], ebx
    mov [cpu_brand + 8], ecx
    mov [cpu_brand + 12], edx
    
    mov eax, 0x80000003
    cpuid
    mov [cpu_brand + 16], eax
    mov [cpu_brand + 20], ebx
    mov [cpu_brand + 24], ecx
    mov [cpu_brand + 28], edx
    
    mov eax, 0x80000004
    cpuid
    mov [cpu_brand + 32], eax
    mov [cpu_brand + 36], ebx
    mov [cpu_brand + 40], ecx
    mov [cpu_brand + 44], edx
    mov byte [cpu_brand + 48], 0
    
    mov esp, ebp
    pop ebp
    ret

; Detect memory size
detect_memory:
    push ebp
    mov ebp, esp
    
    ; Use int 0x15, eax = 0xE820 for memory detection
    mov eax, 0xE820
    xor ebx, ebx
    mov ecx, 24
    mov edx, 0x534D4150  ; 'SMAP'
    mov edi, memory_size
    int 0x15
    
    ; Store total memory size
    mov eax, [edi + 8]   ; Length of memory region
    mov [memory_size], eax
    
    mov esp, ebp
    pop ebp
    ret

; Detect disk size
detect_disk:
    push ebp
    mov ebp, esp
    
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
    mov [disk_size], eax
    jmp .done
    
.error:
    mov dword [disk_size], 0
    
.done:
    mov esp, ebp
    pop ebp
    ret

; Print decimal number in EAX
print_dec:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    
    mov ecx, 10
    mov ebx, esp        ; Use stack as buffer
    
.convert_loop:
    xor edx, edx
    div ecx             ; Divide by 10
    add dl, '0'         ; Convert to ASCII
    dec ebx
    mov [ebx], dl
    test eax, eax
    jnz .convert_loop
    
.print_loop:
    mov al, [ebx]
    call print_char
    inc ebx
    cmp ebx, esp
    jb .print_loop
    
    pop edx
    pop ecx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

; Print hex byte in AL
print_hex_byte:
    push ebp
    mov ebp, esp
    
    push eax
    shr al, 4
    call print_hex_digit
    pop eax
    and al, 0x0F
    call print_hex_digit
    
    mov esp, ebp
    pop ebp
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