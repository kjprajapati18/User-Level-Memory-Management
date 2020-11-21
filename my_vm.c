#include "my_vm.h"

void* pMem = NULL;
char* pBitMap, *vBitMap;
pde_t* pageDir;
int offsetBits, pdtBits, ptBits, pdtSize, ptSize;

pthread_mutex_t mapLock;
//#define debug

/*
Function responsible for allocating and setting your physical memory 
*/
void SetPhysicalMem() {

    pthread_mutex_init(&mapLock, NULL);

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
    memset(pageDir, 0, sizeof(pde_t)*pdtSize);
    /*
    int i;
    for(i = 0; i < pdtSize; i++){
        pageDir[i] = malloc(sizeof(pte_t)*ptSize);
    }*/
    
    int pBMSize = sizeof(char)*MEMSIZE/PGSIZE;
    int vBMSize = sizeof(char)*MAX_MEMSIZE/PGSIZE;
    pBitMap = malloc(pBMSize);
    memset(pBitMap, 0, pBMSize);
    vBitMap = malloc(vBMSize);
    memset(vBitMap, 0, vBMSize);
}



/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t *Translate(pde_t *pgdir, void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
    unsigned long pdInd = (unsigned long)va >> (offsetBits + ptBits);
    unsigned long ptInd = ((unsigned long)va << pdtBits) >> (pdtBits + offsetBits);
    unsigned long offset = ((unsigned long)va <<pdtBits + ptBits) >> (pdtBits + ptBits);
    #ifdef debug
    printf("pd: %u\n", pdInd);
    printf("pt: %u\n", ptInd);
    #endif
    if(pdInd < pdtSize && ptInd < ptSize){
        pte_t* entry = (pte_t*)(pageDir[pdInd] + ptInd*sizeof(pte_t));
        return (pte_t*) (*entry + offset);
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
    
    //Instead of checkign bit map, map all VA's to 0. Now to check if mapping exists, see if it maps to 0
    //Mapping to 0 means no mapping exists. Mapping to anything else means it exists
    if(checkMap(vBitMap, pdInd, ptInd) == 0){
        pde_t* dir = &(pageDir[pdInd]);
        if(*dir == 0){
            *dir = (pde_t) malloc(sizeof(pte_t)*ptSize);
            memset((void*)(*dir), 0, sizeof(pte_t)*ptSize);
        }
        pte_t* entry = (pte_t*) (*dir + ptInd*sizeof(pte_t)); 
        *entry = (pte_t) pa; //////
        return 0;
    }
    return -1;
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
    //Use virtual address bitmap to find the next free page
    unsigned long i;
    unsigned long entries = pdtSize*ptSize;
    for(i = 0; i < entries; i++){
        //Find free page
        if(vBitMap[i] == 0){
            unsigned long j;
            //Check if enough contiguous free pages
            for(j=1; j < num_pages; j++){
                if(vBitMap[i+j] == 1){
                    i += j;
                    break;
                }
            }
            if(j == num_pages){
                //Generate the virtual address, since page i is good spot
                return (void*) i;
            }
        }
    }
    return (void*) -1;
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *myalloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.
    if(pMem == NULL) SetPhysicalMem();
    /* HINT: If the page directory is not initialized, then initialize the
   page directory. Next, using get_next_avail(), check if there are free pages. If
   free pages are available, set the bitmaps and map a new page. Note, you will 
   have to mark which physical pages are used. */
   
    //Calculate number of pages needed for this malloc
    int numPages = num_bytes%PGSIZE == 0? num_bytes/PGSIZE : num_bytes/PGSIZE + 1;
   
    //Find first free page in vMem that can store this many bytes
    pthread_mutex_lock(&mapLock);
    int pageInd = (int) get_next_avail(numPages);
    //set virtual bitmap for threadsafety, busy wait w global?
    #ifdef debug
    printf("pageInd: %d\n", pageInd);
    #endif
    if(pageInd == -1){
        pthread_mutex_unlock(&mapLock);
        return NULL;
    }
    //Get return address
    void* vaStart = getVA(pageInd);

    //Setup for loop (in case we need to malloc multiple pages)
    void* va = vaStart;
    int physPage = 0;
    int pagesMalloc = 0;
    unsigned long long numEntries = MEMSIZE/PGSIZE;
    while(num_bytes > 0){
        int i;
        //Find any free phys Page
        for(i = physPage; i < numEntries; i++){
            if(pBitMap[i] == 0){
                physPage = i;
                break;
            }
        }
        
        //If no more free phys pages, free everything we have malloced so far and return null
        if(i == numEntries){
            //free virtual bitmap from current pageInd to pageInd-pageMalloc + numPages <--If implemeting thread safety as above
            pthread_mutex_unlock(&mapLock);
            myfree(vaStart, pagesMalloc*PGSIZE - 1); //this pages malloc needs to be converted to a total size
            return NULL;
        }

        //Otherwise, get pa. Map. Set bitmaps.
        void* pa = physPage*PGSIZE+pMem;
        PageMap(pageDir, va, pa);
        pBitMap[physPage] = 1;
        vBitMap[pageInd] = 1;

        //Increase va page index, num pages malloced. Point va to next page if there is still more to malloc
        pageInd++;
        pagesMalloc++;
        num_bytes = num_bytes > PGSIZE? num_bytes-PGSIZE: 0;
        if(num_bytes > 0) va += PGSIZE;
    }
    printBitmap(vBitMap, (unsigned long long)MAX_MEMSIZE/PGSIZE);
    printBitmap(pBitMap, (unsigned long long)MEMSIZE/PGSIZE);
    pthread_mutex_unlock(&mapLock);
    return vaStart;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
int myfree(void *va, int size) {

    //Free the page table entries starting from this virtual address (va)
    // Also mark the pages free in the bitmap
    //Only free if the memory from "va" to va+size is valid
    unsigned long max = -1;

    //If va+size-1 > max, then we want to return. (EX: if u free 1 byte at VA 9 with max 9, it should work)
    if((unsigned long)va > max - size +1 || size <= 0) return -2;
    
    int numPages = size%PGSIZE == 0? size/PGSIZE : size/PGSIZE + 1;
    int pdInd = (unsigned long)va >> (offsetBits + ptBits);
    int ptInd = ((unsigned long)va << pdtBits) >> (pdtBits + offsetBits);
    int i = numPages;
    int j = ptInd;
    int k = pdInd;
    pthread_mutex_lock(&mapLock);
    int check;
    for(check = pdInd*ptSize+ptInd; check < pdInd*ptSize+ptInd + numPages; check++){
        //If any of the attempted page frees were already free, then don't free at all
        if(vBitMap[check] == 0){
            pthread_mutex_unlock(&mapLock);
            return -1;
        }
    }
    while(i > 0){
        //pageDir[k] will give back the memory address of the page entry table
        //Use pointer arithmetic to find the correct entry within that table 
        //(guranteed to be there since bitmap is set + we are within bounds)
        pte_t* entry = (pte_t*) (pageDir[k] + j*sizeof(pte_t));
        pte_t pa = *entry;
        *entry = 0;
        int physInd = (int) (pa - (unsigned long) pMem)/PGSIZE;
        pBitMap[physInd] = 0;
        vBitMap[k*ptSize + j]= 0;
        j++;
        if (j>= ptSize){
            j = 0; 
            k++;
        }
        i--;
    }
    printBitmap(vBitMap, (unsigned long long)MAX_MEMSIZE/PGSIZE);
    printBitmap(pBitMap, (unsigned long long)MEMSIZE/PGSIZE);
    pthread_mutex_unlock(&mapLock);
    #ifdef debug
    printf("Successful free at address %ld\n", va);
    #endif
    return 0;
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
//needs to deep copy with memcopy
//change for loop bounds to while
//size condition for memcpy
void PutVal(void *va, void *val, int size) {

    /* HINT: Using the virtual address and Translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using Translate()
       function.*/
    //go to phys address. set *phys = *val. 
    int numPages = size%PGSIZE == 0? size/PGSIZE : size/PGSIZE + 1;
    unsigned long i = getPageNum(va), j;
    unsigned long entries = pdtSize *ptSize;
    int pagesMalloc = 0;
    unsigned long offset = ((unsigned long)va <<(pdtBits + ptBits)) >> (pdtBits + ptBits);

    if(i + numPages >= entries) return;

    pthread_mutex_lock(&mapLock);
    for(j=i;j< numPages; j++){
        if(vBitMap[j] == 0){
            pthread_mutex_unlock(&mapLock);
            return;
        }
    }

    while(pagesMalloc < numPages){
        pte_t* pa = Translate(pageDir, va); //checknull?
        //hourglass figure
        int len = size > (PGSIZE)? PGSIZE: size;
        if(pagesMalloc == 0) len = size > (PGSIZE-offset)? PGSIZE-offset: size;
        memcpy(pa, val, len);
        val+=len;
        size-= len;
        va+=len;
        pagesMalloc++;

        /*if(pagesMalloc == 0){
            int len = size > (PGSIZE-offset)? PGSIZE-offset: size; 
            memcpy(pa, val, len);
            val+=len;
            size-= len;
            va+=len;
            
            if(size > PGSIZE) {
                memcpy(pa, val, PGSIZE - offset);
                val+=PGSIZE-offset;
                size-=PGSIZE-offset;
            }
            else {
                memcpy(pa, val, size - offset);
                val+=size-offset;
                size-=offset;
            }
        }
        else{
            int len = size > PGSIZE? PGSIZE: size; 
            memcpy(pa, val, len);
            val+=len;
            size-= len;
            va+=len;
            if(size < PGSIZE) memcpy(pa, val, size);
        else {
            memcpy(pa, val, PGSIZE);
            size-=PGSIZE;
            val+=PGSIZE;
        }
        }
        //i++;
        pagesMalloc++;*/
    }
    pthread_mutex_unlock(&mapLock);
    return;
}


/*Given a virtual address, this function copies the contents of the page to val*/
void GetVal(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    "val" address. Assume you can access "val" directly by derefencing them.
    If you are implementing TLB,  always check first the presence of translation
    in TLB before proceeding forward */
    int numPages = size%PGSIZE == 0? size/PGSIZE: size/PGSIZE+1;
    unsigned long i = getPageNum(va), j = i;
    unsigned long entries = pdtSize*ptSize;
    int pagesFound = 0;
    unsigned long offset = ((unsigned long)va <<pdtBits + ptBits) >> (pdtBits + ptBits);
    if(i + numPages >= entries) return;

    pthread_mutex_lock(&mapLock);
    for(j; j < numPages; j++){
        if(vBitMap[j] == 0){
            pthread_mutex_unlock(&mapLock);
            return;
        }
    }
    while(pagesFound < numPages){
        pte_t* pa = Translate(pageDir, va); //checknull?

        int len = size > (PGSIZE)? PGSIZE: size;
        if(pagesFound == 0) len = size > (PGSIZE-offset)? PGSIZE-offset: size;

        memcpy(val, pa, len);

        val+=len;
        size-= len;
        va+=len;
        pagesFound++;
        /*
        if(pagesFound == 0){
            if(size > PGSIZE) {
                memcpy(val, pa, PGSIZE - offset);
                val+=PGSIZE - offset;
                size-=PGSIZE - offset;
            }
            else {
                memcpy(val, pa, size -offset);
                val+=size-offset;
                size-=offset;   
            }
        }
        else if(size < PGSIZE) memcpy(val, pa, size);
        else {
            memcpy(pa, val, PGSIZE);
            size-=PGSIZE;
            val+=PGSIZE;
        }
        i++;
        pagesFound++;*/
    }
    pthread_mutex_unlock(&mapLock);
    return;
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
    int row, col, i;
    int mat1Ele = 0, mat2Ele = 0;
    int sum = 0;


    for(row = 0; row < size; row++){
        for(col = 0; col < size; col++){
            //Select start of row in mat1, and start of col in mat2
            //Answer gets placed in the corresponding row,col in answer
            sum = 0;
            void* mat1Ind = mat1 + row*size * sizeof(int);
            void* mat2Ind = mat2 + col * sizeof(int);
            void* ansInd = answer + (row*size + col) * sizeof(int);
            
            for(i=0; i < size; i++){
                mat1Ele = 0;
                mat2Ele = 0;
                //Get the ith element of the row (mat1) and ith element of the col (mat2)
                GetVal(mat1Ind + i*sizeof(int), &mat1Ele, sizeof(int));
                GetVal(mat2Ind + i*size*sizeof(int), &mat2Ele, sizeof(int));
                //Add all products to finish the dot product of this row&col
                sum += mat1Ele*mat2Ele;
            }
            //Product found, place it
            PutVal(ansInd, &sum, sizeof(int));
        }
    }
}


//Helper Functions
int checkMap(char* map, int pdInd, int ptInd){
    
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

    return (void*) (pdInd | ptInd);
}

unsigned long getPageNum(void* va){
    unsigned long pdInd = (unsigned long)va >> (offsetBits + ptBits);
    unsigned long ptInd = ((unsigned long)va << pdtBits) >> (pdtBits + offsetBits);
    unsigned long pageNum = pdInd*ptSize + ptInd;
    return pageNum;
}

void printBitmap(char* bm, unsigned int size){
    /*int i;
    if(size < (unsigned long long) MAX_MEMSIZE/PGSIZE) printf("P ");
    else printf("V ");
    printf("Map: ");
    for(i=0; i < size; i++){
        if(bm[i] == 1) printf("%d, ", i);
    }
    printf("done\n");*/
}