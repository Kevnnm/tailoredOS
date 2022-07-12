#include "IDT.h"
#include "IOPorts.h"
#include "print.h"
#include "serial.h"
#include <string.h>

#define IDT_MAX_DESCRIPTORS 256

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESCRIPTORS]; // Create an array of IDT entries; aligned for performance

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

typedef struct{
	isr_t handler;
	void* data;
} isr_data_pair;

static idtr_t idtr;

// General exception handler
void isr_handler(int int_num, register_context* regs) {
	(void) regs;
	serial_write("Interrupt number: ");
	char num_str[4];
	itoa(int_num, num_str, 10);
	serial_write(num_str);
	serial_write("\r\n");
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
	idt_entry_t* descriptor = &idt[vector];

	descriptor->isr_low     = (uint64_t)isr & 0xFFFF; // Lower 16 bits of addr
	descriptor->isr_mid     = ((uint64_t)isr >> 16) & 0xFFFF; // Higher 16 bits of the lower 32 bits of addr
	descriptor->isr_high     = ((uint64_t)isr >> 32) & 0xFFFFFFFF; // Higher 32 bits of addr
	descriptor->kernel_cs   = 0x08; // offset of kernel code selector
	descriptor->ist         = 0; // zero for now TODO: see what IST does
	descriptor-> attributes = flags;
	descriptor->reserved    = 0; // reserved set to zero
}

void irq_handler(void);
void irq_handler(void) {
	__asm__("cli");
}

extern void* isr_stub_table[];

#define PIC1		  0x20		/* IO base address for master PIC */
#define PIC2		  0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA	  (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA	  (PIC2+1)

void disable_pic() {
	// remap IRQ's to avoid conflicts with intel reserved CPU exceptions

    outb(PIC1_COMMAND, 0x11); // 0x11 Initialization command
    outb(PIC2_COMMAND, 0x11); // See: https://wiki.osdev.org/PIC#Initialisation
	outb(PIC1_DATA, 0xF0); // Remap IRQs on both PIC's to 0xF0-0xF8
	outb(PIC2_DATA, 0xF0);
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0xFF); // Mask all interrupts to disable
    outb(PIC2_DATA, 0xFF);
}

void idt_init() {
	idtr.base = (uintptr_t) &idt[0];
	idtr.limit = (uint16_t) sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (uint64_t vector = 0; vector < 256; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E); // Interrupt gate desc.(p=1b, dpl=00b, type=1110b)
	}

	__asm__ volatile ("lidt %0" : : "m"(idtr)); // load new IDT location
	__asm__ volatile ("sti"); // set interrupt flag (IF in EFLAGS register)
}
