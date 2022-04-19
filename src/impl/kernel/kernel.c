#include "print.h"
#include "CPU.h"

void test_interrupt();

void kernel_main() {
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to our 64-bit kernel!\n");
    __asm__("int %0\n" : : "N"(0));

    print_str("back in kernel main\n");
}
