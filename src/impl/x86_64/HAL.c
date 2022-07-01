#include "IDT.h"
#include "APIC.h"
#include "kernel.h"
#include "print.h"
#include "memory.h"

void init_core(void) {
    init_virtual_memory();

    __asm__ volatile ("cli");
    idt_init();
}

void init_extra(void) {
    apic_init();
}


void init_multiboot2(void) {
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);

    init_core();
    /* init_extra(); */

    kernel_main();
}
