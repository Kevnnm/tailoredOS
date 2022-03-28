#include "CPU.h"

cpuid_info_t CPUID() {
    cpuid_info_t info;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;

    __asm__ volatile ("cpuid" : "=b"(ebx), "=d"(edx), "=c"(ecx) : "a"(0)); // Get vendor string
    for (uint8_t i = 0; i < 4; i++)
        info.vendorString[i] = (ebx >> (8 * i)) & 0xFF;
    for (uint8_t i = 0; i < 4; i++)
        info.vendorString[i+4] = (edx >> (8 * i)) & 0xFF;
    for (uint8_t i = 0; i < 4; i++)
        info.vendorString[i+8] = (ecx >> (8 * i)) & 0xFF;

    info.nullTerminator = '\0';

    __asm__ volatile ("cpuid" : "=b"(ebx), "=d"(edx), "=c"(ecx) : "a"(1)); // Get features
    info.features_ecx = ecx;
    info.features_edx = edx;

    return info;
}
