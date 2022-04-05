#include "APIC.h"
#include "CPU.h"
#include "IDT.h"
#include "print.h"


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

    __asm__("sti");

    return 0;
}

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
    uint64_t low = (val & 0xFFFFFFFF) | IA32_APIC_BASE_MSR_ENABLE; // automatically enable
    uint64_t high = val >> 32;
    __asm__ volatile("wrmsr" ::"a"(low), "d"(high), "c"(IA32_APIC_BASE_MSR));
}

uint32_t read_reg(uint32_t off) { return *((volatile uint32_t*)(local_base + off)); }

void write_reg(uint32_t off, uint32_t val) { *((volatile uint32_t*)(local_base + off)) = val; }

int local_init() {
    local_base = read_base() & LOCAL_APIC_BASE;

    write_base(read_base());

    write_reg(LOCAL_APIC_SIVR, read_reg(LOCAL_APIC_SIVR) | 0x1FF);

    return 0;
}
