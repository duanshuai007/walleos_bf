OUTPUT_FORMAT("elf32-littlearm","elf32-bigarm","elf32-littlearm")
OUTPUT(arm)
ENTRY(_stext)

SECTIONS
{
    . = 0x00000000;
    . = ALIGN(4);
    .text :
    {
        PROVIDE(_stext = .);
        ./startup/start.o (.text)
        *(.text)
        *(.text*)

        . = ALIGN(4);
        PROVIDE(_etext = .);
    }

    .data :
    {
        PROVIDE (__data_start = .);
        *(.data)
        *(.data*)
        . = ALIGN(4);
        PROVIDE (__data_end = .);
    }

    .ARM.exidx :
    {   
        . = ALIGN(4);
        PROVIDE (__exidx_start = .); 
        *(.ARM.exidex*)
        PROVIDE (__exidx_end = .); 
    }  

    .bss :
    {   
        PROVIDE(__bss_start = .); 
        *(.bss)
        *(.bss.*)
        PROVIDE(__bss_end = .); 
    } 
}
