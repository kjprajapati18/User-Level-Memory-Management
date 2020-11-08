#include "my_vm.h"

void* pMem = NULL;
int* pBitMap, *vBitMap;
pde_t* pageDir;
int offsetBits, pdtBits, ptBits, pdtSize, ptSize;

//#define debug

/*
Function responsible for allocating and setting your physical memory 
*/
void SetPhysicalMem() {

    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating
    pMem = malloc(MEMSIZE);
    
    offsetBits = (int) (log(PGSIZE)/log(2)); //Relying on compiler to perform the substitution so we dont have to attach library
    pdtBits = (32-offsetBits)/2;
    ptBits = 32-offsetBits-pdtBits;
    
    pdtSize = 1 << pdtBits;
    ptSize = 1 << ptBits;
    #ifdef debug
    printf("pdtSize : %d, \t ptSize: %d\n", pdtSize, ptSize);
    #endif
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them
    pageDir = malloc(sizeof(pde_t) * pdtSize);
    
    int i;
    for(i = 0; i < pdtSize; i++){
        pageDir[i] = malloc(sizeof(pte_t)*ptSize);
    }
    
    int pBMSize = sizeof(int)*pdtSize*ptSize;
    int vBMSize = sizeof(int)*MAX_MEMSIZE/PGSIZE;
    pBitMap = malloc(pBMSize);
    memset(pBitMap, 0, pBMSize);
    vBitMap = malloc(vBMSize);
    memset(vBitMap, 0, vBMSize);
}



/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t * Translate(pde_t *pgdir, void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
    int pdInd = (unsigned long)va >> (offsetBits + ptBits);
    int ptInd = ((unsigned long)va << pdtBits) >> (pdtBits + offsetBits);
    
    #ifdef debug
    printf("pd: %d\n", pdInd);
    printf("pt: %d\n", ptInd);
    #endif
    if(pdInd < pdtSize && ptInd < ptSize){
        pte_t* entry = pageDir[pdInd] + ptInd*sizeof(pte_t);
        return entry;
    }

    //If translation not successfull
    return NULL; 
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int
PageMap(pde_t *pgdir, void *va, void *pa)
{

    /*HINT: Similar to Translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */
    
    int pdInd = (unsigned long)va >> (offsetBits + ptBits);
    int ptInd = ((unsigned long)va << pdtBits) >> (pdtBits + offsetBits);
    if(checkMap(vBitMap, pdInd, ptInd) == 0){
        pte_t* entry = pageDir[pdInd] + ptInd*sizeof(pte_t); 
        *entry = pa; //////
        vBitMap[pdInd*ptSize+ptInd] = 1;
        return 0;
    }
    return -1;
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
    //Use virtual address bitmap to find the next free page
    int i;
    int entries = pdtSize*ptSize;
    for(i = 0; i < entries; i++){
        //Find free page
        if(vBitMap[i] == 0){
            int j;
            //Check if enough contiguous free pages
            for(j=1; j < num_pages; j++){
                if(vBitMap[i+j] == 1){
                    i += j;
                    break;
                }
            }
            if(j == num_pages){
                //Generate the virtual address, since page i is good spot
                return getVA(i);
            }
        }
    }
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *myalloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.

   /* HINT: If the page directory is not initialized, then initialize the
   page directory. Next, using get_next_avail(), check if there are free pages. If
   free pages are available, set the bitmaps and map a new page. Note, you will 
   have to mark which physical pages are used. */

    return NULL;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void myfree(void *va, int size) {

    //Free the page table entries starting from this virtual address (va)
    // Also mark the pages free in the bitmap
    //Only free if the memory from "va" to va+size is valid
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void PutVal(void *va, void *val, int size) {

    /* HINT: Using the virtual address and Translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using Translate()
       function.*/

}


/*Given a virtual address, this function copies the contents of the page to val*/
void GetVal(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    "val" address. Assume you can access "val" directly by derefencing them.
    If you are implementing TLB,  always check first the presence of translation
    in TLB before proceeding forward */


}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void MatMult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
    matrix accessed. Similar to the code in test.c, you will use GetVal() to
    load each element and perform multiplication. Take a look at test.c! In addition to 
    getting the values from two matrices, you will perform multiplication and 
    store the result to the "answer array"*/

       
}


//Helper Functions
int checkMap(int* map, int pdInd, int ptInd){
    
    if(pdInd < pdtSize && ptInd < ptSize){
       return map[pdInd*ptSize+ptInd];
    }
    return -1;
}

void* getVA(int pageNum){
    pte_t pdInd = pageNum/ptSize;
    pte_t ptInd = pageNum%ptSize;

    pdInd = pdInd <<(offsetBits+ptBits);
    ptInd = ptInd << offsetBits;

    return (pdInd | ptInd);
}