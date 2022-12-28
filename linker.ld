ENTRY(_start)
OUTPUT_FORMAT("binary")
SECTIONS {
    . = 0x7c00;
    .text :
    {
        *(._start)
        *(.text)
	. = ALIGN(4096);
    }
    .data :
    {
        *(.data)
	. = ALIGN(4096);
    }
    .bss :
    {
        *(.bss)
	. = ALIGN(4096);
    }
}