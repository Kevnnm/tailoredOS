global long_mode_main
extern kernel_main

section .text
bits 64                         ; Use 64 bit instructions
long_mode_main:
    ;; load null into all data segment registers
    mov   ax, 0
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax


    call kernel_main
    hlt
