#include "my_vm.h"
#include <string.h>
#include <stdio.h>


void* pm;
struct tlb tlb_store;

void set_bit(char *bitmap, int index) {
    unsigned int manip = bitmap[index / 8]; //useless line cause empty but keep it anyway
    unsigned int bit = 1 << index % 8;
    manip = manip | bit;
    bitmap[index / 8] = manip;
}

int get_bit(char *bitmap, int index) {
    unsigned int manip = bitmap[index / 8];
    manip = manip >> (index % 8);
    return manip % 2;
}

unsigned int get_top_bits(unsigned int value,  int num_bits, int bitmap_size)
{
    unsigned int cross = ((int) (2 << num_bits) - 1) << (8 * bitmap_size - num_bits);
    
    unsigned int gimme = value & cross; //Removing all but the top bits

    return gimme >> 8 * bitmap_size - num_bits; //moving bits to get top bit
}

void insert_ipt() {

    static int num_ipts = 0;

    for (int i = 0; i < NUM_PHYS_BYTES_NEEDED; i++) {
        
        unsigned char bastard = P_BITMAP[i];
        bastard ^= 255;
        
        int result = 0;
        while(bastard >>= 1) result++;
        
        if (result) {
            set_bit(P_BITMAP, (i*8) + result);
            pde_t address = ((i*8)+(7-result)*PGSIZE);
            OPT[num_ipts] = address;
            num_ipts++;
            break;
        }
    
    }
    
}

/*
Function responsible for allocating and setting your physical memory 
*/
void set_physical_mem() {

    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating

    //Allocate the PHYSICAL MEMORY, 1GB
    pm = malloc(MEMSIZE);
    
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them
    for (int i = 0; i < NUM_PHYS_BYTES_NEEDED; i++) {
        P_BITMAP[i] &= 0x0;
    }

    for (int i = 0; i < NUM_VIRT_BYTES_NEEDED; i++) {
        V_BITMAP[i] &= 0x0;
    }

    for (int i = 0; i < (START/8); i++) {
        P_BITMAP[i] ^= 255;
    }
    
    insert_ipt();
    
}

void free_the_ting() {
    free(pm);
}


/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 * 
 * Note: Make sure this is thread safe by locking around critical 
 *       data structures touched when interacting with the TLB
 */
int TLB_add(void *va, void *pa) { //Finished

    tlb_store.va[tlb_counter] = va;
    tlb_store.pa[tlb_counter] = pa;

    tlb_counter++;

    if(tlb_counter == TLB_ENTRIES) tlb_counter = 0;

    return -1;
}


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 * 
 * Note: Make sure this is thread safe by locking around critical 
 *       data structures touched when interacting with the TLB
 */
pte_t * TLB_check(void *va) {

    /* Part 2: TLB lookup code here */

    for(int i = 0; i < TLB_ENTRIES; i++)
        if(tlb_store.va[i] == va) 
            return tlb_store.pa[i]; /*This function should return a pte_t pointer*/
   
   return NULL;
}


/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */
void print_TLB_missrate() {
    double miss_rate = 0;	

    /*Part 2 Code here to calculate and print the TLB miss rate*/




    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}

//bruh

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
void *translate(pde_t *pgdir, void *va) { //only called when the va is valid. To check if the va is valid, call map_page
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
    * 2nd-level-page table index using the virtual address.  Using the page
    * directory index and page table index get the physical address.
    *
    * Part 2 HINT: Check the TLB before performing the translation. If
    * translation exists, then you can return physical address from the TLB.
    */

   if (map_page(pgdir, va, ___)) {
    return phys_add
   }

   // mutex lock set_phys_memory()


    //If translation not successful, then return NULL
    return NULL; 
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int map_page(pde_t *pgdir, void *va, void *pa) {

    /*HINT: Similar to translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */

    /*
    1. check if va is valid
    - check if it is in opt
        - no: then go to map the page
        - yes: check if it is in ipt:
            - no: then go to map the page
            - yes: return 1
    2. map the page:
    - 
    
    */

    return -1;
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {

    int curr_cont = 0;
    int curr_index_cont = -1; //holds the virtual page entry address

    //Use virtual address bitmap to find the next free page
    for(int i = 0; i < NUM_VIRT_BYTES_NEEDED/8; i++){
        if(V_BITMAP[i] != 255){ //if there exists a 0 in this bitmap index

            for(int j = 0; j < 8; j++){ //get each bit and count if num_pages fit here
                if(get_bit(V_BITMAP, (i*8) + j) == 0){ //if free spot found
                    if(curr_index_cont == -1){ //if start of contiguous, assign index variable to address of the start
                        //assign curr_index_cont to the virtual page entry address
                        curr_cont++;
                    }else if(curr_index_cont != -1){ //if contiguous, curr_cont++
                        curr_cont++;
                    }

                    //If successfully found contiguous entries, return start of contiguous
                    if(num_pages == curr_cont){
                        set_bit(V_BITMAP, (i*8) + j);
                        return curr_index_cont;
                    }
                    
                }else{ //if occupied spot found, if index == 1 basically
                    if(curr_index_cont == -1){ //if not contiguous and still nothing found
                        //do nothing
                    }else if(curr_index_cont != -1){ //if contiguous but hits an occupied spot, RESET
                        curr_index_cont = -1;
                        curr_cont = 0;
                    }
                }
            }

            curr_cont = 0;
            curr_index_cont = -1;

        }
    }

    return NULL;
}


/* Function responsible for allocating pages and used by the benchmark
 *
 * Note: Make sure this is thread safe by locking around critical 
 *       data structures you plan on updating. Two threads calling
 *       this function at the same time should NOT get the same
 *       or overlapping allocations
*/
void *n_malloc(unsigned int num_bytes) {

    /* 
     * HINT: If the physical memory is not yet initialized, then allocate and initialize.

       set_physical_mem(MEMSIZE);

     */

    /*
    

    needs to call get_next_avail to know where to allocate the pages. First need to calculate # of pages.

    1. Find an open page(s) in V_BITMAP and "set those bits" --> get their REAL addys (get_next_avail)
    2. Find open physical pages to map the virtual pages --> get their REAL addys
    3. For each virtual addy given by get_nextavail:
        - map_page(pm, real_virt_add, real_phys_add) // going to fail --> returns 0 and maps the virt addys to the phys addys

    */

    

   /* 
    * HINT: If the page directory is not initialized, then initialize the
    * page directory. Next, using get_next_avail(), check if there are free pages. If
    * free pages are available, set the bitmaps and map a new page. Note, you will 
    * have to mark which physical pages are used. 
    */

    return NULL;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void n_free(void *va, int size) {

    /* Part 1: Free the page table entries starting from this virtual address
     * (va). Also mark the pages free in the bitmap. Perform free only if the 
     * memory from "va" to va+size is valid.
     *
     * Part 2: Also, remove the translation from the TLB
     */
    
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
 * The function returns 0 if the put is successfull and -1 otherwise.
*/
int put_data(void *va, void *val, int size) {

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
     * the contents of "val" to a physical page. NOTE: The "size" value can be larger 
     * than one page. Therefore, you may have to find multiple pages using translate()
     * function.
     */

    // call translate, but before calling translate set up a separate helper to check if va is valid --> check if va is in the tlb first.


    /*return -1 if put_data failed and 0 if put is successfull*/

    return -1;
}


/*Given a virtual address, this function copies the contents of the page to val*/
void get_data(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    * "val" address. Assume you can access "val" directly by derefencing them.
    */

}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
     * matrix accessed. Similar to the code in test.c, you will use get_data() to
     * load each element and perform multiplication. Take a look at test.c! In addition to 
     * getting the values from two matrices, you will perform multiplication and 
     * store the result to the "answer array"
     */
    int x, y, val_size = sizeof(int);
    int i, j, k;
    for (i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            unsigned int a, b, c = 0;
            for (k = 0; k < size; k++) {
                int address_a = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                int address_b = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));
                get_data( (void *)address_a, &a, sizeof(int));
                get_data( (void *)address_b, &b, sizeof(int));
                // printf("Values at the index: %d, %d, %d, %d, %d\n", 
                //     a, b, size, (i * size + k), (k * size + j));
                c += (a * b);
            }
            int address_c = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            // printf("This is the c: %d, address: %x!\n", c, address_c);
            put_data((void *)address_c, (void *)&c, sizeof(int));
        }
    }
}



