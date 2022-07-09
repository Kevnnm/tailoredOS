#include "serial.h"
#include "IOPorts.h"
#include <string.h>

enum SerialPorts {
COM1 = 0x3F8
};

#define PORT COM1

void init_serial() {
    // Mostly using Serial for kernel debugging to log to external device before crashes.
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_writec(const char c) {
    while (!(inb(PORT + 5) & 0x20)) // PORT + 5 = Line Status Register
        ; // Make sure we are not transmitting data

    outb(PORT, c);
}

void serial_writen(const char* s, unsigned n) {
    while (n--) {
        while (!(inb(PORT + 5) & 0x20)) // PORT + 5 = Line Status Register
            ; // Make sure we are not transmitting data
        outb(PORT, *s++);
    }
}

void serial_write(const char* s) { serial_writen(s, strlen(s)); }
