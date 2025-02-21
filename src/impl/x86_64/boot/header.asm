section .multiboot_header    ; Multiboot2 header section

header_start:
    dd 0xe85250d6           ; Multiboot2 magic number
    dd 0                    ; Architecture: protected mode i386
    dd header_end - header_start    ; Header length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))    ; Checksum

    ; Required end tag
    dw 0                    ; Type: end tag
    dw 0                    ; Flags (none)
    dd 8                    ; Size of tag (8 bytes)

header_end:                 ; End of multiboot header
