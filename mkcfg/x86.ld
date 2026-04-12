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
        //. = ALIGN(4096);
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
        //. = ALIGN(4096);
    }
    
    .data : {
        *(.data)
        *(.data.*)
        //. = ALIGN(4096);
    }
    
    .bss : {
        __bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        . = ALIGN(4096);
        __bss_end = .;
    }

    .weak : {
        *(.weak)
    }

    .idt : {
        *(.idt)
    }

    . = ALIGN(4);
    __boot_loader_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.note)
    }
}