#include "IDT.h"
#include "APIC.h"
#include "kernel.h"
#include "print.h"

void init_core(void) {
    __asm__ volatile ("cli");
    idt_init();
}

void init_extra(void) {
    apic_init();
}


void init_multiboot2(void) {
    init_core();

    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);

    init_extra();

    kernel_main();
}
