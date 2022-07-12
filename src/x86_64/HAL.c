#include "IDT.h"
#include "APIC.h"
#include "kernel.h"
#include "print.h"
#include "memory.h"
#include "serial.h"

void init_core(void) {
    init_virtual_memory();

    __asm__ volatile ("cli");
    idt_init();
}

void init_extra(void) {
    apic_init();
}


void init_multiboot2(void) {
    init_serial();
    serial_write("Starting tailoredOS...\r\n");

    init_core();
    init_extra();

	__asm__ volatile ("sti"); // set interrupt flag (IF in EFLAGS register)

    kernel_main();
}
