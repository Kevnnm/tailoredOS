#include "APIC.h"
#include "CPU.h"
#include "IDT.h"
#include "print.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_ENABLE 0x800

uintptr_t local_base;

uint64_t read_base() {
    uint64_t low;
    uint64_t high;
    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(IA32_APIC_BASE_MSR));

    return (high << 32) | low;
}

void write_base(uint64_t val) {
    uint64_t low = (val & 0xFFFFFFFF);
    uint64_t high = val >> 32;
    __asm__ volatile("wrmsr" ::"a"(low), "d"(high), "c"(IA32_APIC_BASE_MSR));
}

// FIXME: Page faulting because we have not page mapped IO Memory
// The plan is to map the first 4GB of physical memory to KERNEL_VIRTUAL_BASE - 4GB
// (KERNEL_VIRTUAL_BASE - 0x100000000ULL) and provide a function GetIOMemory to access
// the physical pages. Point 4 PDPT entries to 4 PDT's with page sized, present and writable
// and cache disabled bits set for 2 MB page translations. (512 * 2MB) * 4 = 4GB
//
// Make a note in the initialization of virtual memory that the kernel page maps are
// already setup during the initial boot.
#define APIC_READ(off) *((volatile uint32_t*)(local_base + off))
#define APIC_WRITE(off, val) (*((volatile uint32_t*)(local_base + off)) = val)

int local_init() {
    local_base = read_base() & LOCAL_APIC_BASE;

    write_base(read_base() | IA32_APIC_BASE_MSR_ENABLE);

    // Set bit 8 to of Spurious Interrupt Vector Register to enable APIC
    // and set the spurious interrupt number to 255.
    APIC_WRITE(LOCAL_APIC_SIVR, APIC_READ(LOCAL_APIC_SIVR) | 0x1FF);

    // TODO: Create a spurious interrupt handler and register it with the
    // IDT handler at index 0xFF with it.

    return 0;
}

int apic_init() {
    cpuid_info_t cpu_info = CPUID();

    if (!(cpu_info.features_edx & CPUID_EDX_APIC)) {
        print_str("APIC not present\n");
        return 1;
    }

    __asm__("cli");

    disable_pic();

    // TODO: init local APIC and IO-APIC
    // https://wiki.osdev.org/APIC
    /* local_init(); */

    __asm__("sti");

    return 0;
}
