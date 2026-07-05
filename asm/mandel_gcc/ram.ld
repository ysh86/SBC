ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = _start_addr, LENGTH = 64K - _start_addr
}

SECTIONS
{
    .text ORIGIN(RAM) :
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
