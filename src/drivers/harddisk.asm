[BITS 32]
; Hard Disk Driver for NansOS
; Supports ATA/IDE drives

; ATA ports
ATA_PRIMARY_DATA        equ 0x1F0
ATA_PRIMARY_ERROR      equ 0x1F1
ATA_PRIMARY_SECTORS    equ 0x1F2
ATA_PRIMARY_LBA_LOW    equ 0x1F3
ATA_PRIMARY_LBA_MID    equ 0x1F4
ATA_PRIMARY_LBA_HIGH   equ 0x1F5
ATA_PRIMARY_DRIVE      equ 0x1F6
ATA_PRIMARY_STATUS     equ 0x1F7
ATA_PRIMARY_COMMAND    equ 0x1F7

; ATA commands
ATA_CMD_READ_SECTORS   equ 0x20
ATA_CMD_WRITE_SECTORS  equ 0x30
ATA_CMD_IDENTIFY       equ 0xEC

; Status register bits
ATA_SR_BSY    equ 0x80    ; Busy
ATA_SR_DRDY   equ 0x40    ; Drive ready
ATA_SR_DRQ    equ 0x08    ; Data request ready
ATA_SR_ERR    equ 0x01    ; Error

section .data
    hd_buffer:       times 512 db 0   ; Sector buffer
    hd_info:         times 512 db 0   ; Drive information buffer
    hd_present:      db 0             ; Is hard drive present?
    hd_size:         dq 0             ; Drive size in sectors
    hd_model:        times 41 db 0    ; Drive model string
    hd_serial:       times 21 db 0    ; Drive serial number

section .text
global hd_init
global hd_read_sector
global hd_write_sector
global hd_get_info
global hd_get_size

hd_init:
    push ebp
    mov ebp, esp
    pushad
    
    ; Software reset
    mov dx, ATA_PRIMARY_COMMAND
    mov al, 0x04
    out dx, al
    
    ; Wait for drive to be ready
    call hd_wait_ready
    
    ; Send IDENTIFY command
    mov dx, ATA_PRIMARY_COMMAND
    mov al, ATA_CMD_IDENTIFY
    out dx, al
    
    ; Wait for data or error
    call hd_wait_ready
    
    ; Check if drive exists
    mov dx, ATA_PRIMARY_STATUS
    in al, dx
    test al, ATA_SR_ERR
    jnz .no_drive
    
    ; Read identify data
    mov ecx, 256        ; 256 words = 512 bytes
    mov dx, ATA_PRIMARY_DATA
    mov edi, hd_info
.read_loop:
    in ax, dx
    mov [edi], ax
    add edi, 2
    loop .read_loop
    
    ; Drive is present
    mov byte [hd_present], 1
    
    ; Extract drive information
    mov esi, hd_info + 27*2  ; Model number starts at word 27
    mov edi, hd_model
    mov ecx, 20             ; 20 words = 40 bytes
.copy_model:
    mov ax, [esi]
    mov [edi], ah           ; Swap bytes (big endian to little endian)
    mov [edi + 1], al
    add esi, 2
    add edi, 2
    loop .copy_model
    mov byte [hd_model + 40], 0  ; Null terminate
    
    ; Get drive size
    mov eax, [hd_info + 60*2]    ; LBA sectors
    mov [hd_size], eax
    
    popad
    mov esp, ebp
    pop ebp
    xor eax, eax        ; Return success
    ret
    
.no_drive:
    mov byte [hd_present], 0
    popad
    mov esp, ebp
    pop ebp
    mov eax, 1          ; Return error
    ret

hd_read_sector:
    push ebp
    mov ebp, esp
    pushad
    
    ; Parameters:
    ; ebp + 8  = LBA
    ; ebp + 12 = buffer
    
    ; Check if drive is present
    cmp byte [hd_present], 0
    je .error
    
    ; Wait for drive to be ready
    call hd_wait_ready
    
    ; Set up registers
    mov dx, ATA_PRIMARY_SECTORS
    mov al, 1                   ; Read one sector
    out dx, al
    
    mov eax, [ebp + 8]         ; LBA
    mov dx, ATA_PRIMARY_LBA_LOW
    out dx, al
    shr eax, 8
    mov dx, ATA_PRIMARY_LBA_MID
    out dx, al
    shr eax, 8
    mov dx, ATA_PRIMARY_LBA_HIGH
    out dx, al
    shr eax, 8
    and al, 0x0F                ; Only lower 4 bits
    or al, 0xE0                 ; Set LBA mode, drive 0
    mov dx, ATA_PRIMARY_DRIVE
    out dx, al
    
    ; Send read command
    mov dx, ATA_PRIMARY_COMMAND
    mov al, ATA_CMD_READ_SECTORS
    out dx, al
    
    ; Wait for data
    call hd_wait_ready
    
    ; Read data
    mov ecx, 256               ; 256 words = 512 bytes
    mov dx, ATA_PRIMARY_DATA
    mov edi, [ebp + 12]       ; Buffer
.read_loop:
    in ax, dx
    mov [edi], ax
    add edi, 2
    loop .read_loop
    
    popad
    mov esp, ebp
    pop ebp
    xor eax, eax             ; Return success
    ret
    
.error:
    popad
    mov esp, ebp
    pop ebp
    mov eax, 1               ; Return error
    ret

hd_write_sector:
    push ebp
    mov ebp, esp
    pushad
    
    ; Parameters:
    ; ebp + 8  = LBA
    ; ebp + 12 = buffer
    
    ; Check if drive is present
    cmp byte [hd_present], 0
    je .error
    
    ; Wait for drive to be ready
    call hd_wait_ready
    
    ; Set up registers
    mov dx, ATA_PRIMARY_SECTORS
    mov al, 1                   ; Write one sector
    out dx, al
    
    mov eax, [ebp + 8]         ; LBA
    mov dx, ATA_PRIMARY_LBA_LOW
    out dx, al
    shr eax, 8
    mov dx, ATA_PRIMARY_LBA_MID
    out dx, al
    shr eax, 8
    mov dx, ATA_PRIMARY_LBA_HIGH
    out dx, al
    shr eax, 8
    and al, 0x0F                ; Only lower 4 bits
    or al, 0xE0                 ; Set LBA mode, drive 0
    mov dx, ATA_PRIMARY_DRIVE
    out dx, al
    
    ; Send write command
    mov dx, ATA_PRIMARY_COMMAND
    mov al, ATA_CMD_WRITE_SECTORS
    out dx, al
    
    ; Wait for drive ready to accept data
    call hd_wait_ready
    
    ; Write data
    mov ecx, 256               ; 256 words = 512 bytes
    mov dx, ATA_PRIMARY_DATA
    mov esi, [ebp + 12]       ; Buffer
.write_loop:
    mov ax, [esi]
    out dx, ax
    add esi, 2
    loop .write_loop
    
    ; Flush cache
    mov dx, ATA_PRIMARY_COMMAND
    mov al, 0xE7               ; Cache flush command
    out dx, al
    
    call hd_wait_ready
    
    popad
    mov esp, ebp
    pop ebp
    xor eax, eax              ; Return success
    ret
    
.error:
    popad
    mov esp, ebp
    pop ebp
    mov eax, 1                ; Return error
    ret

hd_get_info:
    push ebp
    mov ebp, esp
    
    ; Return drive information
    mov eax, hd_model         ; Return pointer to model string
    
    mov esp, ebp
    pop ebp
    ret

hd_get_size:
    push ebp
    mov ebp, esp
    
    ; Return drive size
    mov eax, [hd_size]
    mov edx, [hd_size + 4]
    
    mov esp, ebp
    pop ebp
    ret

; Wait for drive to be ready
hd_wait_ready:
    push ecx
    mov ecx, 100000          ; Timeout counter
.loop:
    mov dx, ATA_PRIMARY_STATUS
    in al, dx
    test al, ATA_SR_BSY
    jz .ready
    loop .loop
    pop ecx
    stc                      ; Set carry flag (timeout)
    ret
.ready:
    pop ecx
    clc                      ; Clear carry flag (success)
    ret 