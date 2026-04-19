OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS {
    . = 0x10000;

    __sys_base_addr = .;

    .sys : {
        *(.sys)
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