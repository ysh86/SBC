ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64K
}

SECTIONS
{
    .vectors 0x00000000 :
    {
        LONG(_ssp_top)
        LONG(_start)
        . = _start_addr;
    } > RAM

    .text _start_addr :
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
