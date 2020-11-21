#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need
#include <math.h>
#include <string.h>
#include <pthread.h>

#define PGSIZE 1024

// Maximum size of your memory
#define MAX_MEMSIZE 4ULL*1024*1024*1024 //4GB

#define MEMSIZE 1024*4ULL     //1GB

// Represents a page table entry
typedef unsigned long pte_t;

// Represents a page directory entry
typedef unsigned long pde_t;

#define TLB_SIZE 120

//Structure to represents TLB
typedef struct _tlb {

    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries)
    // You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
    void* va;
    void* pa;

}tlb;

tlb* tlb_store;


void SetPhysicalMem();
pte_t* Translate(pde_t *pgdir, void *va);
int PageMap(pde_t *pgdir, void *va, void* pa);
int check_in_tlb(void *va); //was bool im gonna change to int cuz fuck that
void put_in_tlb(void *va, void *pa);
void *myalloc(unsigned int num_bytes);
int myfree(void *va, int size);
void PutVal(void *va, void *val, int size);
void GetVal(void *va, void *val, int size);
void MatMult(void *mat1, void *mat2, int size, void *answer);

//Helper Function headers
int checkMap(char* map, int pdInd, int ptInd);
void* getVA(int page);
unsigned long getPageNum(void* va);
void printBitmap(char* bm, unsigned int size);

#endif
