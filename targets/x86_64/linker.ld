ENTRY(_start)

SECTIONS
{
  . = 1M;

  .rodata BLOCK(4K) : ALIGN(4K)
  {
    KEEP(*(.multiboot_header))
    *(.rodata)
    *(.rodata*)
  }

  .text BLOCK(4K) : ALIGN (4K)
  {
    *(.text)
    *(.text*)
  }

  .bss BLOCK(4K) : ALIGN(4K)
  {
    *(.bss)
  }
}
