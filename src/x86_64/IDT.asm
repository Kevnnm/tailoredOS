section .text
%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro ISR_COMMON_EXIT 0
    popaq
    add rsp, 8
    iretq
%endmacro

%macro ISR_ERR_STUB 1
global isr_%1
isr_%1:
    cli
    pushaq
    mov rdi, %1                 ; arg: int_num
    mov rsi, rsp                ; arg: register_context* (top of stack)
    xor rbp, rbp
    call isr_handler
    ISR_COMMON_EXIT
%endmacro

%macro ISR_NO_ERR_STUB 1
global isr_%1
isr_%1:
    cli
    push 0
    pushaq
    mov rdi, %1                 ; arg: int_num
    mov rsi, rsp                ; arg: register_context* (top of stack)
    xor rbp, rbp
    call isr_handler
    ISR_COMMON_EXIT
%endmacro

%macro ISR_STUB_NOP 1
isr_%1:
	iretq
%endmacro

%macro IRQ 2
global irq_%1
irq_%1:
    cli
    push 0
    pushaq
    mov rdi, %2
    mov rsi, rsp
    xor rdx, rdx
    xor rbp, rbp
    call irq_handler
    ISR_COMMON_EXIT
%endmacro

%macro IPI 1
global ipi_%1
ipi_%1:
    cli
    push 0
    pushaq
    mov rdi, %1
    mov rsi, rsp
    xor rbp, rbp
    call ipi_handler
    ISR_COMMON_EXIT
%endmacro

extern isr_handler
extern irq_handler
extern ipi_handler

ISR_NO_ERR_STUB 0
ISR_NO_ERR_STUB 1
ISR_NO_ERR_STUB 2
ISR_NO_ERR_STUB 3
ISR_NO_ERR_STUB 4
ISR_NO_ERR_STUB 5
ISR_NO_ERR_STUB 6
ISR_NO_ERR_STUB 7
ISR_ERR_STUB 8
ISR_NO_ERR_STUB 9
ISR_ERR_STUB 10
ISR_ERR_STUB 11
ISR_ERR_STUB 12
ISR_ERR_STUB 13
ISR_ERR_STUB 14
ISR_NO_ERR_STUB 15
ISR_NO_ERR_STUB 16
ISR_ERR_STUB 17
ISR_NO_ERR_STUB 18
ISR_NO_ERR_STUB 19
ISR_NO_ERR_STUB 20
ISR_NO_ERR_STUB 21
ISR_NO_ERR_STUB 22
ISR_NO_ERR_STUB 23
ISR_NO_ERR_STUB 24
ISR_NO_ERR_STUB 25
ISR_NO_ERR_STUB 26
ISR_NO_ERR_STUB 27
ISR_NO_ERR_STUB 28
ISR_NO_ERR_STUB 29
ISR_ERR_STUB 30
ISR_NO_ERR_STUB 31

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

%assign num 48
%rep 256-48
    IPI num
%assign num (num + 1)
%endrep

section .rodata
global int_vectors
int_vectors:
%assign num 48
%rep 256-48
    dq ipi_%+ num
%assign num (num + 1)
%endrep
