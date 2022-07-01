#pragma once

#include <stdint.h>

#define PAGES_PER_TABLE 512
#define TABLES_PER_DIR 512
#define DIRS_PER_PDPT 512
#define PDPTS_PER_PML4 512

#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000ULL // Kernel resides here
#define IO_VIRTUAL_BASE (KERNEL_VIRTUAL_BASE - 0x100000000ULL) // KERNEL_VIRTUAL_BASE - 4GB

/* Useful macros for getting table indices from virtual address */
#define PML4_GET_INDEX(addr) (((addr) >> 39) & 0x1FF)
#define PDPT_GET_INDEX(addr) (((addr) >> 30) & 0x1FF)
#define PAGE_DIR_GET_INDEX(addr) (((addr) >> 21) & 0x1FF)
#define PAGE_TABLE_GET_INDEX(addr) (((addr) >> 12) & 0x1FF)

typedef uint64_t page_t;
typedef uint64_t pd_entry_t;
typedef uint64_t pdpt_entry_t;
typedef uint64_t pml4_entry_t;

typedef pd_entry_t page_dir_t[TABLES_PER_DIR];
typedef pdpt_entry_t pdpt_t[DIRS_PER_PDPT];
typedef pml4_entry_t pml4_t[PDPTS_PER_PML4];

#define PML4_PRESENT 1
#define PML4_WRITABLE (1 << 1)

#define PDPT_PRESENT 1
#define PDPT_WRITABLE (1 << 1)

#define PDE_PRESENT 1
#define PDE_WRITABLE (1 << 1)
#define PDE_CACHE_DISABLED (1 << 4)
#define PDE_2M (1 << 7)

#define PAGE_PRESENT 1
#define PAGE_WRITABLE (1 << 1)
#define PAGE_CACHE_DISABLED (1 << 4)
#define PAGE_FRAME 0xFFFFFFFFFF000ULL

#define PAGE_SIZE_4K 4096U
#define PAGE_SIZE_2M 0x200000U
#define PAGE_SIZE_1G 0x40000000ULL

static inline void SetPageFrame(uint64_t* page, uint64_t addr) { *page = (*page & ~PAGE_FRAME) | (addr & PAGE_FRAME); }
static inline void SetPageFlags(uint64_t* page, uint64_t flags) { *page |= flags; }

void init_virtual_memory(void);
