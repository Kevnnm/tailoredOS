#include "memory.h"
#include "string.h"

pml4_t kernelPML4 __attribute__((aligned(4096)));
pdpt_t kernelPDPT __attribute__((aligned(4096))); // Kernel itself will reside here (0xFFFFFFFF80000000)
page_dir_t kernelDir __attribute__((aligned(4096)));
page_dir_t ioDirs[4] __attribute__((aligned(4096)));

void init_virtual_memory(void) {
    memset(kernelPML4, 0, sizeof(pml4_t));
    memset(kernelPDPT, 0, sizeof(pdpt_t));

    SetPageFrame(&(kernelPML4[PML4_GET_INDEX(KERNEL_VIRTUAL_BASE)]), ((uint64_t)kernelPDPT - KERNEL_VIRTUAL_BASE));
    kernelPML4[PML4_GET_INDEX(KERNEL_VIRTUAL_BASE)] |= 0x3;
    kernelPML4[0] = kernelPML4[PML4_GET_INDEX(KERNEL_VIRTUAL_BASE)];

    kernelPDPT[PDPT_GET_INDEX(KERNEL_VIRTUAL_BASE)] = ((uint64_t)kernelDir - KERNEL_VIRTUAL_BASE) | 0x3;
    for (int j = 0; j < TABLES_PER_DIR; j++) {
        kernelDir[j] = (PAGE_SIZE_2M * j) | 0x83;
    }
    kernelPDPT[0] =
        kernelPDPT[PDPT_GET_INDEX(KERNEL_VIRTUAL_BASE)];

    for (int i = 0; i < 4; i++) {
        kernelPDPT[PDPT_GET_INDEX(IO_VIRTUAL_BASE) + i] =
            ((uint64_t)ioDirs[i] - KERNEL_VIRTUAL_BASE) | 0x3; //(PAGE_SIZE_1G * i) | 0x83;
        for (int j = 0; j < TABLES_PER_DIR; j++) {
            ioDirs[i][j] =
                (PAGE_SIZE_1G * i + PAGE_SIZE_2M * j) | (PDE_2M | PDE_WRITABLE | PDE_PRESENT | PDE_CACHE_DISABLED);
        }
    }

    __asm__ volatile("mov %%rax, %%cr3" ::"a"((uint64_t)kernelPML4 - KERNEL_VIRTUAL_BASE));
}

uintptr_t get_io_mapping(uintptr_t addr) {
    if (addr > 0xffffffff) {
        // MMIO generally does not reside > 4GB
        return 0xffffffff;
    }

    return addr + IO_VIRTUAL_BASE;
}
