/* Linker script for NansOS 64-bit */
OUTPUT_FORMAT(pei-x86-64)
ENTRY(_start)

/* PE format requirements */
IMAGE_BASE = 0x100000;
SECTIONS
{
    . = SIZEOF_HEADERS;
    . = ALIGN(4096);
    .text . + IMAGE_BASE : AT(IMAGE_BASE)
    {
        _text_start = .;
        *(.text)
        *(.text.*)
        . = ALIGN(4096);
        _text_end = .;
    }

    .rdata : ALIGN(4096)
    {
        _rdata_start = .;
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4096);
        _rdata_end = .;
    }

    .data : ALIGN(4096)
    {
        _data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4096);
        _data_end = .;
    }

    .bss : ALIGN(4096)
    {
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4096);
        _bss_end = .;
    }

    .idata : ALIGN(4096)
    {
        _idata_start = .;
        *(.idata)
        . = ALIGN(4096);
        _idata_end = .;
    }

    .reloc : ALIGN(4096)
    {
        _reloc_start = .;
        *(.reloc)
        . = ALIGN(4096);
        _reloc_end = .;
    }

    _end = .;

    /* Discard unnecessary sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
        *(.pdata)
        *(.xdata)
        *(.debug*)
    }
} 