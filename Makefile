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

x86_64_asm_sources := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_objects := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%_asm.o, $(x86_64_asm_sources))

x86_64_objects := $(x86_64_c_objects) $(x86_64_asm_objects)

includes := -I src/include/kernel \
			-I src/include/x86_64

$(kernel_objects) : build/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
    printf "\e33;1mCompiling c file\e[0m $@\n" && \
	x86_64-elf-gcc -std=c11 -c $(includes) -ffreestanding $(patsubst build/kernel/%.o, src/impl/kernel/%.c, $@) -o $@

$(x86_64_c_objects) : build/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
    printf "\e33;1mCompiling c file\e[0m $@\n" && \
	x86_64-elf-gcc -c $(includes) -ffreestanding $(patsubst build/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

$(x86_64_asm_objects) : build/x86_64/%_asm.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
    printf "\e[33;1mCompiling assembly file\e[0m $@\n" && \
	nasm -f elf64 $(patsubst build/x86_64/%_asm.o, src/impl/x86_64/%.asm, $@) -o $@

.PHONY: build-x86_64 clean

build-x86_64: $(kernel_objects) $(x86_64_objects)
	mkdir -p dist/x86_64 . && \
    printf "\e[33;1mLinking into\e[0m $(KERNELBIN)\n" && \
	x86_64-elf-ld -n -o $(KERNELBIN) -T targets/x86_64/linker.ld $(kernel_objects) $(x86_64_objects) && \
	cp $(KERNELBIN) targets/x86_64/iso/boot/kernel.bin && \
    printf "\e[33;1mCreating\e[0m $(ISO)\n" && \
	grub-mkrescue /usr/lib/grub/i386-pc -o $(ISO) targets/x86_64/iso && \
    printf "\e[33;1mDone\e[0m\n"

clean:
	rm -r dist/ build/
	find targets/ -name kernel.bin -delete
# end

