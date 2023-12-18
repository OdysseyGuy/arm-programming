OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* pretend two memory areas as ROM and RAM */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x60000000, LENGTH = 1M
    RAM (rwx) : ORIGIN = 0x70000000, LENGTH = 32M
}

SECTIONS
{
    /*
        Text section read-only, fixed-size
        Put the vector_table section at 0x0 then code section
     */
    .text : {
        _text_start = .;
        *(.vector_table)
        *(.text*)
        /* todo: store in a separate .rodata section */
        *(.rodata*)  /* store read-only data section with text in rom */
    } > FLASH
    _text_end = .;

    /*
        Data section read-write
     */
    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        _data_start = .;
        *(.data)
        . = ALIGN(8);
        _data_end = .;
    } > RAM

    /*
        BSS (block started by symbol) section
        Needs to be filled with zeros
     */
    .bss :
    {
        _bss_start = .;
        *(.bss)
        . = ALIGN(8);
        _bss_end = .;
    } > RAM

    /DISCARD/ : {
      *(.comment)
      *(.note*)
    }

    /* processor mode stacks */
    _fiq_stack_start = ADDR(.bss) + SIZEOF(.bss);
    _fiq_stack_end = _fiq_stack_start + 0x1000; /* 64 KiB */

    _irq_stack_start = _fiq_stack_end;
    _irq_stack_end = _irq_stack_start + 0x1000; /* 64 KiB */

    _stack_start = _irq_stack_end;
    _stack_end = _stack_start + 0x1000;         /* 64 KiB */

    _fiq_stack_size = _fiq_stack_end - _fiq_stack_start;
    _irq_stack_size = _irq_stack_end - _irq_stack_start;
}