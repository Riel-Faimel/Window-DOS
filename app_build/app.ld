OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS {    
    . = 0x500;

    .exCOMHead : {
        LONG(0x4D4F4345)
        LONG(_start)
        LONG(code_fileoff)
        LONG(ADDR(.text))
        LONG(SIZEOF(.text) + SIZEOF(.init_array) + SIZEOF(.fini_array))
        LONG(data_fileoff)
        LONG(ADDR(.data))
        LONG(SIZEOF(.data))
        LONG(SIZEOF(.bss))
    }
    
    code_fileoff = . - 0x500;
    .text : {
        *(.text)
        *(.text.*)
    }
    
    .init_array : {
        __init_array_start = .;
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
        __init_array_end = .;
    }
    .fini_array : {
        __fini_array_start = .;
        KEEP(*(.fini_array))
        KEEP(*(.fini_array.*))
        __fini_array_end = .;
    }
    
    . = ALIGN(4);
    data_fileoff = . - 0x500;
    .data : {
        *(.data)
        *(.data.*)
        *(.rodata)
        *(.rodata.*)
    }
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.note)
        *(.note.*)
    }
}