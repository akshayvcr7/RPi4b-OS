ENTRY(_start)

SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x80000 - 0x400;
    /* For AArch64, use . = 0x80000; */
    __start = .;
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    /*. = ALIGN(4096); /* align to page size */
    __text_end = .;

    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    /*. = ALIGN(4096); /* align to page size */
    __rodata_end = .;

    __data_start = .;
    .data :
    {
        *(.data)
    }
    /*. = ALIGN(4096); /* align to page size */
    __data_end = .;

    __bss_start = .;
    .bss :
    {
        . = ALIGN(16);
        bss = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }
    /*. = ALIGN(4096); /* align to page size */
    __end = .;
    __bss_size = (__bss_end - __bss_start) >> 3;
    __load_size = (__end - __start) >> 3;
    
}


    /*0x8000 for 32-bit kernels ("arm_64bit=1" in config.txt not set)
    0x80000 for older 64-bit kernels ("arm_64bit=1" set, flat image)
    0x200000 for newer 64-bit kernels ("arm_64bit=1" set, gzip'ed Linux ARM64 image)
    0x0 if "kernel_old=1" set*/
