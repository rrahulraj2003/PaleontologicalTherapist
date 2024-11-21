#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need

#define PGSIZE 4096

#define MEMSIZE 1024*1024*1024 // Size of "physcial memory"
#define NUM_PHYS_BITS_NEEDED (MEMSIZE/PGSIZE) // # of physical bits needed = # of pages in physical memory
#define NUM_PHYS_BYTES_NEEDED (NUM_PHYS_BITS_NEEDED/8) // # of bytes needed in physical memory to store the physical bitmap
#define NUM_PHYS_BITMAP_PAGES (NUM_PHYS_BYTES_NEEDED/PGSIZE) // # of pages in physical memory needed to store the physical bitmap

//#define MAX_MEMSIZE 4ULL*1024*1024*1024
#define MAX_MEMSIZE 2147483647 // Maximum size of virtual memory
#define NUM_VIRT_BITS_NEEDED (MAX_MEMSIZE/PGSIZE) // # of virtual bits needed = # of pages in virtual address space
#define NUM_VIRT_BYTES_NEEDED (NUM_VIRT_BITS_NEEDED/8) // # of bytes needed in physical memory to store the virtual bitmap
#define NUM_VIRT_BITMAP_PAGES (NUM_VIRT_BYTES_NEEDED/PGSIZE) // # of pages in physical memory needed to store the virtual bitmap

// Represents a page table entry
typedef unsigned long pte_t;

// Represents a page directory entry
typedef unsigned long pde_t;

#define TLB_ENTRIES 512
#define offset 12

//Physical memory
extern void* pm;

#define OPT ((pde_t*)pm)
#define P_BITMAP ((unsigned char *)pm + PGSIZE)
#define V_BITMAP ((unsigned char *)pm + PGSIZE + (NUM_PHYS_BITMAP_PAGES * PGSIZE))
#define START PGSIZE + (NUM_PHYS_BITMAP_PAGES * PGSIZE) + (NUM_VIRT_BITMAP_PAGES * PGSIZE)

#define TOTAL_OPT_ENTRIES 1 << (32-offset)/2
#define TOTAL_IPT_ENTRIES ((32-offset) % 2 == 0) ? 1 << ((32-offset)/2) : 1 << ((32-offset)/2 + 1) 



//Structure to represents TLB
struct tlb {
    /*Assume your TLB is a direct mapped TLB with number of entries as TLB_ENTRIES
    * Think about the size of each TLB entry that performs virtual to physical
    * address translation.
    */
   pde_t va[TLB_ENTRIES];
   pte_t pa[TLB_ENTRIES];
};
extern struct tlb tlb_store;

unsigned long tlb_counter = 0;

// Setup functions
void set_physical_mem();
void free_the_ting();

// TLB Functions
int TLB_add(void *va, void *pa);
pte_t *TLB_check(void *va);
void print_TLB_missrate();

// Page Table Functions
void* translate(pde_t *pgdir, void *va);
int map_page(pde_t *pgdir, void *va, void* pa);

// Allocation functions
void *n_malloc(unsigned int num_bytes);
void n_free(void *va, int size);

// Data operations
int put_data(void *va, void *val, int size);
void get_data(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);

#endif
