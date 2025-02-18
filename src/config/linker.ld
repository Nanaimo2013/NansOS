/* Linker script for NansOS kernel */
ENTRY(_start)
OUTPUT_FORMAT(pei-i386)

SECTIONS
{
    . = 0x1000;

    .text : {
        *(.text.startup)
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
        *(.note.gnu.build-id)
        *(.gnu.hash)
        *(.dynsym)
        *(.dynstr)
        *(.gnu.version*)
        *(.rel*)
        *(.plt*)
    }
}