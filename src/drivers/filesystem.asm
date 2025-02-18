[BITS 32]
; Filesystem Driver for NansOS
; Supports FAT12 filesystem

extern print_char    ; Add external declaration

; FAT12 constants
BYTES_PER_SECTOR    equ 512
SECTORS_PER_CLUSTER equ 1
RESERVED_SECTORS    equ 1
NUMBER_OF_FATS      equ 2
ROOT_ENTRIES        equ 224
SECTORS_PER_FAT     equ 9
SECTORS_PER_TRACK   equ 18
NUMBER_OF_HEADS     equ 2

section .data
    current_directory:  times 256 db 0  ; Current directory path
    fat_buffer:        times 4608 db 0  ; Buffer for FAT (9 sectors * 512 bytes)
    dir_buffer:        times 512 db 0   ; Buffer for directory entries
    file_buffer:       times 512 db 0   ; Buffer for file operations
    drive_number:      db 0             ; Current drive number

section .text
global fs_init
global fs_read_sector
global fs_write_sector
global fs_open_file
global fs_read_file
global fs_write_file
global fs_create_file
global fs_delete_file
global fs_list_directory
global fs_change_directory

fs_init:
    push ebp
    mov ebp, esp
    
    ; Store drive number
    mov al, [ebp + 8]
    mov [drive_number], al
    
    ; Read first FAT sector
    push dword 1          ; Sector number (first FAT sector)
    push dword fat_buffer ; Buffer
    call fs_read_sector
    add esp, 8
    
    ; Read root directory
    push dword 19         ; First root directory sector
    push dword dir_buffer
    call fs_read_sector
    add esp, 8
    
    mov esp, ebp
    pop ebp
    ret

fs_read_sector:
    push ebp
    mov ebp, esp
    pushad
    
    ; Parameters:
    ; ebp + 8  = buffer
    ; ebp + 12 = sector number
    
    ; Calculate CHS address
    mov eax, [ebp + 12]   ; Sector number
    xor edx, edx
    mov ebx, SECTORS_PER_TRACK
    div ebx               ; EAX = Track, DL = Sector
    
    inc dl               ; Sectors start at 1
    push dx             ; Save sector number
    
    xor edx, edx
    mov ebx, NUMBER_OF_HEADS
    div ebx              ; EAX = Cylinder, DL = Head
    
    ; Set up disk read
    mov ah, 0x02        ; Read sectors
    mov al, 1           ; Number of sectors to read
    mov ch, al          ; Cylinder number
    mov cl, [esp]       ; Sector number
    mov dh, dl          ; Head number
    mov dl, [drive_number] ; Drive number
    mov bx, [ebp + 8]   ; Buffer address
    
    int 0x13
    jc .error
    
    add esp, 2          ; Clean up stack
    popad
    mov esp, ebp
    pop ebp
    xor eax, eax       ; Return success
    ret
    
.error:
    add esp, 2
    popad
    mov esp, ebp
    pop ebp
    mov eax, 1         ; Return error
    ret

fs_write_sector:
    push ebp
    mov ebp, esp
    pushad
    
    ; Parameters:
    ; ebp + 8  = buffer
    ; ebp + 12 = sector number
    
    ; Calculate CHS address
    mov eax, [ebp + 12]   ; Sector number
    xor edx, edx
    mov ebx, SECTORS_PER_TRACK
    div ebx               ; EAX = Track, DL = Sector
    
    inc dl               ; Sectors start at 1
    push dx             ; Save sector number
    
    xor edx, edx
    mov ebx, NUMBER_OF_HEADS
    div ebx              ; EAX = Cylinder, DL = Head
    
    ; Set up disk write
    mov ah, 0x03        ; Write sectors
    mov al, 1           ; Number of sectors to write
    mov ch, al          ; Cylinder number
    mov cl, [esp]       ; Sector number
    mov dh, dl          ; Head number
    mov dl, [drive_number] ; Drive number
    mov bx, [ebp + 8]   ; Buffer address
    
    int 0x13
    jc .error
    
    add esp, 2          ; Clean up stack
    popad
    mov esp, ebp
    pop ebp
    xor eax, eax       ; Return success
    ret
    
.error:
    add esp, 2
    popad
    mov esp, ebp
    pop ebp
    mov eax, 1         ; Return error
    ret

fs_open_file:
    push ebp
    mov ebp, esp
    pushad
    
    ; Parameters:
    ; ebp + 8 = filename
    
    ; Search root directory for file
    mov esi, [ebp + 8]    ; Filename
    mov edi, dir_buffer
    mov ecx, ROOT_ENTRIES
    
.search_loop:
    push ecx
    push esi
    push edi
    mov ecx, 11          ; Compare 11 characters (8.3 format)
    repe cmpsb
    pop edi
    pop esi
    je .found
    
    add edi, 32          ; Move to next directory entry
    pop ecx
    loop .search_loop
    
    ; File not found
    popad
    mov esp, ebp
    pop ebp
    mov eax, 0xFFFFFFFF  ; Return error
    ret
    
.found:
    ; Get first cluster
    mov ax, [edi + 26]   ; First cluster number
    
    popad
    mov esp, ebp
    pop ebp
    ret

fs_read_file:
    push ebp
    mov ebp, esp
    pushad
    
    ; Parameters:
    ; ebp + 8  = cluster number
    ; ebp + 12 = buffer
    ; ebp + 16 = max bytes to read
    
    mov esi, [ebp + 8]   ; Current cluster
    mov edi, [ebp + 12]  ; Buffer
    mov ecx, [ebp + 16]  ; Bytes remaining
    
.read_cluster:
    ; Calculate sector number
    mov eax, esi
    sub eax, 2           ; First two clusters are reserved
    mov ebx, SECTORS_PER_CLUSTER
    mul ebx
    add eax, 31          ; First data sector
    
    ; Read cluster
    push eax
    push edi
    call fs_read_sector
    add esp, 8
    
    ; Update counters
    add edi, BYTES_PER_SECTOR
    sub ecx, BYTES_PER_SECTOR
    jle .done
    
    ; Get next cluster from FAT
    mov eax, esi
    mov ebx, 3
    mul ebx
    shr eax, 1           ; Divide by 2
    mov ebx, eax
    mov ax, [fat_buffer + ebx]
    
    test esi, 1
    jz .even_cluster
    
    shr ax, 4           ; Odd cluster
    jmp .check_end
    
.even_cluster:
    and ax, 0x0FFF      ; Even cluster
    
.check_end:
    cmp ax, 0xFF8       ; End of chain?
    jae .done
    
    mov esi, eax        ; Next cluster
    jmp .read_cluster
    
.done:
    popad
    mov esp, ebp
    pop ebp
    ret

fs_write_file:
    ; TODO: Implement file writing
    ret

fs_create_file:
    ; TODO: Implement file creation
    ret

fs_delete_file:
    ; TODO: Implement file deletion
    ret

fs_list_directory:
    push ebp
    mov ebp, esp
    pushad
    
    ; Read root directory
    push dword 19         ; First root directory sector
    push dword dir_buffer
    call fs_read_sector
    add esp, 8
    
    ; Print directory entries
    mov esi, dir_buffer
    mov ecx, ROOT_ENTRIES
    
.list_loop:
    ; Check if entry is used
    cmp byte [esi], 0    ; Empty entry?
    je .next_entry
    cmp byte [esi], 0xE5 ; Deleted entry?
    je .next_entry
    
    ; Print filename
    push ecx
    mov ecx, 8           ; Print 8 characters of name
.name_loop:
    mov al, [esi]
    cmp al, ' '
    je .extension
    push eax
    call print_char      ; Use display driver to print
    add esp, 4
    inc esi
    loop .name_loop
    
.extension:
    mov al, '.'
    push eax
    call print_char
    add esp, 4
    
    add esi, 8           ; Skip to extension
    mov ecx, 3           ; Print 3 characters of extension
.ext_loop:
    mov al, [esi]
    cmp al, ' '
    je .entry_done
    push eax
    call print_char
    add esp, 4
    inc esi
    loop .ext_loop
    
.entry_done:
    ; Print newline
    mov al, 13
    push eax
    call print_char
    add esp, 4
    mov al, 10
    push eax
    call print_char
    add esp, 4
    
    pop ecx
    
.next_entry:
    add esi, 32          ; Move to next entry
    loop .list_loop
    
    popad
    mov esp, ebp
    pop ebp
    ret

fs_change_directory:
    ; TODO: Implement directory change
    ret 