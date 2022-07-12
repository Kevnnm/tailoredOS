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

void irq_handler(int int_num, register_context* regs) {
    (void) int_num;
    (void) regs;
    serial_write("In IRQ handler\r\n");
    return;
}

void ipi_handler(int int_num, register_context* regs) {
    (void) int_num;
    (void) regs;
    serial_write("In IPI handler\r\n");
    return;
}

extern void* int_vectors[];
void isr_0();
void isr_1();
void isr_2();
void isr_3();
void isr_4();
void isr_5();
void isr_6();
void isr_7();
void isr_8();
void isr_9();
void isr_10();
void isr_11();
void isr_12();
void isr_13();
void isr_14();
void isr_15();
void isr_16();
void isr_17();
void isr_18();
void isr_19();
void isr_20();
void isr_21();
void isr_22();
void isr_23();
void isr_24();
void isr_25();
void isr_26();
void isr_27();
void isr_28();
void isr_29();
void isr_30();
void isr_31();
void irq_0();
void irq_1();
void irq_2();
void irq_3();
void irq_4();
void irq_5();
void irq_6();
void irq_7();
void irq_8();
void irq_9();
void irq_10();
void irq_11();
void irq_12();
void irq_13();
void irq_14();
void irq_15();


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
        idt_set_descriptor(vector, 0, 0x8E); // Interrupt gate desc.(p=1b, dpl=00b, type=1110b)
    }

    for (uint64_t vector = 48; vector < 256; vector++) {
        idt_set_descriptor(vector, int_vectors[vector - 48], 0x8E); // Interrupt gate desc.(p=1b, dpl=00b, type=1110b)
    }

    idt_set_descriptor(0, isr_0, 0x8E);
    idt_set_descriptor(1, isr_1, 0x8E);
    idt_set_descriptor(2, isr_2, 0x8E);
    idt_set_descriptor(3, isr_3, 0x8E);
    idt_set_descriptor(4, isr_4, 0x8E);
    idt_set_descriptor(5, isr_5, 0x8E);
    idt_set_descriptor(6, isr_6, 0x8E);
    idt_set_descriptor(7, isr_7, 0x8E);
    idt_set_descriptor(8, isr_8, 0x8E);
    idt_set_descriptor(9, isr_9, 0x8E);
    idt_set_descriptor(10, isr_10, 0x8E);
    idt_set_descriptor(11, isr_11, 0x8E);
    idt_set_descriptor(12, isr_12, 0x8E);
    idt_set_descriptor(13, isr_13, 0x8E);
    idt_set_descriptor(14, isr_14, 0x8E);
    idt_set_descriptor(15, isr_15, 0x8E);
    idt_set_descriptor(16, isr_16, 0x8E);
    idt_set_descriptor(17, isr_17, 0x8E);
    idt_set_descriptor(18, isr_18, 0x8E);
    idt_set_descriptor(19, isr_19, 0x8E);
    idt_set_descriptor(20, isr_20, 0x8E);
    idt_set_descriptor(21, isr_21, 0x8E);
    idt_set_descriptor(22, isr_22, 0x8E);
    idt_set_descriptor(23, isr_23, 0x8E);
    idt_set_descriptor(24, isr_24, 0x8E);
    idt_set_descriptor(25, isr_25, 0x8E);
    idt_set_descriptor(26, isr_26, 0x8E);
    idt_set_descriptor(27, isr_27, 0x8E);
    idt_set_descriptor(28, isr_28, 0x8E);
    idt_set_descriptor(29, isr_29, 0x8E);
    idt_set_descriptor(30, isr_30, 0x8E);
    idt_set_descriptor(31, isr_31, 0x8E);

	__asm__ volatile ("lidt %0" : : "m"(idtr)); // load new IDT location

    idt_set_descriptor(32, irq_0, 0x8E);
    idt_set_descriptor(33, irq_1, 0x8E);
    idt_set_descriptor(34, irq_2, 0x8E);
    idt_set_descriptor(35, irq_3, 0x8E);
    idt_set_descriptor(36, irq_4, 0x8E);
    idt_set_descriptor(37, irq_5, 0x8E);
    idt_set_descriptor(38, irq_6, 0x8E);
    idt_set_descriptor(39, irq_7, 0x8E);
    idt_set_descriptor(40, irq_8, 0x8E);
    idt_set_descriptor(41, irq_9, 0x8E);
    idt_set_descriptor(42, irq_10, 0x8E);
    idt_set_descriptor(43, irq_11, 0x8E);
    idt_set_descriptor(44, irq_12, 0x8E);
    idt_set_descriptor(45, irq_13, 0x8E);
    idt_set_descriptor(46, irq_14, 0x8E);
    idt_set_descriptor(47, irq_15, 0x8E);
}
