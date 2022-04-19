ENTRY(_start)

phys = 0x100000;
offset = 0xFFFFFFFF80000000;
virt = offset + phys;

SECTIONS
{
  . = virt;

  .rodata ALIGN(4K) : AT(ADDR(.rodata) - offset)
  {
    KEEP(*(.multiboot_header))
    *(.rodata)
    *(.rodata*)
  }

  .text ALIGN(4K) : AT(ADDR(.text) - offset)
  {
    *(.text)
    *(.text*)
  }

  .data ALIGN(4K) : AT(ADDR(.data) - offset)
  {
          *(.data)
  }

  .bss ALIGN(4K) : AT(ADDR(.bss) - offset)
  {
    *(.bss)
  }
}
