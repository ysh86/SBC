ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00000400, LENGTH = 64K - 0x400
}

SECTIONS
{
    .text 0x00000400 :
    {
        KEEP(*(.text.start))
        *(.text*)
        *(.rodata*)
    } > RAM

    .data :
    {
        *(.data*)
    } > RAM

    .bss (NOLOAD) :
    {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    . = ALIGN(2);
    _image_end = .;

    PROVIDE(_ssp_top = ORIGIN(RAM) + LENGTH(RAM));
    PROVIDE(_usp_top = ORIGIN(RAM) + LENGTH(RAM) - 0x1000);
}
