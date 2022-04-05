;; boot.asm was made with the following resources:
;;   - https://en.wikipedia.org/wiki/Control_register
;;   - https://wiki.osdev.org/Long_Mode
;;   - https://wiki.osdev.org/Paging
;;   - https://www.amd.com/system/files/TechDocs/24594.pdf
;;   - https://www.amd.com/system/files/TechDocs/24593.pdf
;;   # TODO: Refactor File

section .text
bits 32                         ; 32 bit mode instructions
global _start

_start:
    mov esp, stack_top          ; esp register holds current stack pointer

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call page_tables
    call enable_paging

    lgdt [GDT64Pointer]
    ;; Reload CS register containing code selector with jmp command
    ;; see https://wiki.osdev.org/GDT_Tutorial#What_to_Put_In_a_GDT
    jmp GDT64.Code:entry64


check_multiboot:
    cmp eax, 0x36d76289        ; Magic value multiboot2 stores into eax when loading OS
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "M"
    jmp error

check_cpuid:
    pushfd                      ; Push CPU flags register
    pop eax
    mov ecx, eax
    xor eax, 1 << 21            ; ID bit is the 21st bit
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "C"
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    ;; Check if highest function parameter is higher than
    ;; what we will use
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29            ; Long-Mode support is indicated by the 29th bit
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "L"
    jmp error

;; Identity mapping for now
;; # TODO: Improve memory mapping
page_tables:
    mov eax, page_directory_pointer_entry
    or eax, 0b11                ; present, writable flags
    mov [page_table_l4], eax

    mov eax, page_directory_entry
    or eax, 0b11                ; present, writable flags
    mov [page_directory_pointer_entry], eax

    mov ecx, 0                  ; index

.loop:
    ;; Pointing Page Directory directly to physical memory instead of to Page Table Entry
    ;; See `AMD64 Architecture Programmer’s Manual Volume 2: System Programming` Chapter 5.3.4
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011             ; present, writable, huge page size (2MiB)
    mov [page_directory_entry + ecx * 8], eax ; PDE holds 2MiB seperated pages

    inc ecx
    ;; 512 * 2MiB = 1GiB of memory mapped
    cmp ecx, 512                ; Check if whole table is mapped
    jne .loop

    ret

enable_paging:
    ;; CPU looks at cr3 for top level page map
    mov eax, page_table_l4
    mov cr3, eax

    ;; Enable PAE
    mov edx, cr4
    or edx, (1 << 5)
    mov cr4, edx

    ;; Enable long-mode
    mov ecx, 0xc0000080
    rdmsr
    or eax, (1 << 8)
    wrmsr

    ;; Enable paging (and protected mode, if it isn't already active)
    mov ebx, cr0
    or ebx, (1 << 31) | (1 << 0)
    mov cr0, ebx

    ret

error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte [0xb800a], al
    hlt

bits 64
extern init_multiboot2
entry64:
    ;; load kernel data segment into all data segment registers
    mov   ax, GDT64.Data
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax

    mov rsp, stack_top

    call init_multiboot2

    cli
    hlt

section .bss
align 4096
page_table_l4:
    resb 4096
page_directory_pointer_entry:
    resb 4096
page_directory_entry:
    resb 4096
stack_bottom:
    resb 4096 * 4
stack_top:

section .rodata
align 16
GDT64:                           ; Global Descriptor Table (64-bit).
.Null: equ $ - GDT64             ; The null descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
.Code: equ $ - GDT64         ; The code descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
.Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.UserCode: equ $ - GDT64     ; The usermode code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11111010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
.UserData: equ $ - GDT64     ; The usermode data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11110010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.TSS: ;equ $ - GDT64         ; TSS Descriptor
.len:
    dw 108                       ; TSS Length - the x86_64 TSS is 108 bytes loong
.low:
    dw 0                         ; Base (low).
.mid:
    db 0                         ; Base (middle)
    db 10001001b                 ; Flags
    db 00000000b                 ; Flags 2
    .high:
    db 0                         ; Base (high).
    .high32:
    dd 0                         ; High 32 bits
    dd 0                         ; Reserved
GDT64Pointer:                    ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.
