OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS {
    . = 0x10000;
    
    .start : {
        *(.start)
    }
    
    .text : {
        *(.text)
        *(.text.*)
        *(.text*)
    }

    .ctors : {
        __CTOR_LIST__ = .;
        LONG(-1)
        *(.ctors)
        *(.ctors.*)
        LONG(0)
        __CTOR_END__ = .;
    } 
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
        *(.rdata*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
    }

    .weak : {
        *(.weak)
    }
    
    .bss : {
        __bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        . = ALIGN(4096);
        __bss_end = .;
    }

    .eh_frame : {
        __eh_frame_start = .;
        KEEP(*(.eh_frame))
        __eh_frame_end = .;
        __gcc_except_table_start = .;
        KEEP(*(.gcc_except_table))
        __gcc_except_table_end = .;
    }

    . = ALIGN(4);
    __boot_loader_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.note)
    }
}