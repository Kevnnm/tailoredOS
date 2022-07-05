##
# Tailored OS
#
# @file
# @version 0.1

ISO 			   := dist/x86_64/kernel.iso
KERNELBIN    	   := dist/x86_64/kernel.bin

kernel_sources := $(shell find src/impl/kernel -name *.c)
kernel_objects := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel_sources))

x86_64_c_sources := $(shell find src/impl/x86_64 -name *.c)
x86_64_c_objects := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.o, $(x86_64_c_sources))

c_sources := $(shell find src/impl -maxdepth 1 -name *.c)
c_objects := $(patsubst src/impl/%.c, build/%.o, $(c_sources))

x86_64_asm_sources := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_objects := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%_asm.o, $(x86_64_asm_sources))

x86_64_objects := $(x86_64_c_objects) $(x86_64_asm_objects)

CFLAGS += -ggdb -std=c11 -ffreestanding -mno-red-zone -mcmodel=kernel
CFLAGS += -Wall -Werror -Wextra
CFLAGS += -mno-sse -mno-mmx -mno-sse2

includes := -I src/include/kernel \
			-I src/include/x86_64 \
			-I src/include \
			-I /usr/include \
			-I /usr/include/x86_64-linux-gnu

$(kernel_objects) : build/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
    printf "\e33;1mCompiling c file\e[0m $@\n" && \
	x86_64-elf-gcc $(CFLAGS) -c $(includes) $(patsubst build/kernel/%.o, src/impl/kernel/%.c, $@) -o $@

$(x86_64_c_objects) : build/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
    printf "\e33;1mCompiling c file\e[0m $@\n" && \
	x86_64-elf-gcc $(CFLAGS) -c $(includes) $(patsubst build/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

$(x86_64_asm_objects) : build/x86_64/%_asm.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
    printf "\e[33;1mCompiling assembly file\e[0m $@\n" && \
	nasm -g -f elf64 -F dwarf $(patsubst build/x86_64/%_asm.o, src/impl/x86_64/%.asm, $@) -o $@

$(c_objects) : build/%.o : src/impl/%.c
	mkdir -p $(dir $@) && \
    printf "\e[33;1mCompiling c file\e[0m $@\n" && \
	x86_64-elf-gcc $(CFLAGS) -c $(includes) $(patsubst build/%.o, src/impl/%.c, $@) -o $@

.PHONY: build-x86_64 clean

build-x86_64: $(kernel_objects) $(x86_64_objects) $(c_objects)
	mkdir -p dist/x86_64 . && \
    printf "\e[33;1mLinking into\e[0m $(KERNELBIN)\n" && \
	x86_64-elf-ld -n -o $(KERNELBIN) -T targets/x86_64/linker.ld $(kernel_objects) $(x86_64_objects) $(c_objects) && \
	cp $(KERNELBIN) targets/x86_64/iso/boot/kernel.bin && \
    printf "\e[33;1mCreating\e[0m $(ISO)\n" && \
	grub-mkrescue /usr/lib/grub/i386-pc -o $(ISO) targets/x86_64/iso && \
    printf "\e[33;1mDone\e[0m\n"

clean:
	rm -r dist/ build/
	find targets/ -name kernel.bin -delete
# end

