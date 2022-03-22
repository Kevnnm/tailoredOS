;;  Made following multiboot2 specs at:
;; `https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html`

section .multiboot_header
align 64
header_start:
    dd 0xE85250D6               ; Multiboot2 magic value
    dd 0x00000000               ; Architecture (protected mode i386)
    dd header_end - header_start ; header length
    ;; checksum
    dd 0x100000000 - (0xE85250D6 + 0x00000000 + (header_end - header_start))

    ;; End tag
    dw 0x0
    dw 0x0
    dd 0x8
header_end:
