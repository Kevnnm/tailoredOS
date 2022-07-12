#pragma once

/*
*  Inline assembly for IO
*/

#include <stdint.h>

#define ALWAYS_INLINE __attribute__(( always_inline )) inline

ALWAYS_INLINE static void outb(uint16_t port, uint8_t value) { __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value)); }

ALWAYS_INLINE static uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

ALWAYS_INLINE static void outw(uint16_t port, uint16_t value) { __asm__ volatile("outw %1, %0" : : "dN"(port), "a"(value)); }

ALWAYS_INLINE static uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

ALWAYS_INLINE static void outl(uint16_t port, uint32_t value) { __asm__ volatile("outl %1, %0" : : "dN"(port), "a"(value)); }

ALWAYS_INLINE static uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
