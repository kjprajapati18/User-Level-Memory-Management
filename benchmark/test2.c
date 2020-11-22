#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../my_vm.h"


#define SIZE 10

struct thing{
    unsigned long* a;
    unsigned long* b;
};

void* matMulT2(void* input){
    struct thing *thing2 = (struct thing*) input;
    void* a = thing2->a;
    void* b = thing2->b;
    void *c = myalloc(4*SIZE*SIZE);
    sleep(1);

    printf("Addresses of the allocations: %x, %x, %x\n", (int)a, (int)b, (int)c);

/*
    int x = 1;

    int i,j;
    int address_a, address_b;
    printf("Storing integers to generate a SIZExSIZE matrix\n");
    for (i = 0; i < SIZE; i++) {
        //printf("%d\n", i);
        for (j = 0; j < SIZE; j++) {
            address_a = (unsigned int)a + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            address_b = (unsigned int)b + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            PutVal((void *)address_a, &x, sizeof(int));
            PutVal((void *)address_b, &x, sizeof(int));
            
        }
    } 
*/
    //printf("Fetching matrix elements stored in the arrays\n");
    /*
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            address_a = (unsigned int)a + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            address_b = (unsigned int)b + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            GetVal((void *)address_a, &y, sizeof(int));
            GetVal( (void *)address_b, &z, sizeof(int));
            printf("%d ", y);
        }
        printf("\n");
    } 
*/
    printf("Performing matrix multiplication with itself!\n");
    MatMult(a, b, SIZE, c);
    return c;
}

void* matMulT(){
    void *a = myalloc(4*SIZE*SIZE);
    sleep(1);
    void *b = myalloc(4*SIZE*SIZE);
    sleep(1);
    void *c = myalloc(4*SIZE*SIZE);
    sleep(1);

    printf("Addresses of the allocations: %x, %x, %x\n", (int)a, (int)b, (int)c);

    int x = 1;

    int i,j;
    int address_a, address_b;
    printf("Storing integers to generate a SIZExSIZE matrix\n");
    for (i = 0; i < SIZE; i++) {
        //printf("%d\n", i);
        for (j = 0; j < SIZE; j++) {
            address_a = (unsigned int)a + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            address_b = (unsigned int)b + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            PutVal((void *)address_a, &x, sizeof(int));
            PutVal((void *)address_b, &x, sizeof(int));
            
        }
    } 

    //printf("Fetching matrix elements stored in the arrays\n");
    /*
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            address_a = (unsigned int)a + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            address_b = (unsigned int)b + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            GetVal((void *)address_a, &y, sizeof(int));
            GetVal( (void *)address_b, &z, sizeof(int));
            printf("%d ", y);
        }
        printf("\n");
    } 
*/
    printf("Performing matrix multiplication with itself!\n");
    MatMult(a, b, SIZE, c);
    return c;
}

void printMat(void* c){
    int i,j,y;
    int address_c;
    for (i = 0; i < SIZE; i++) {
        //printf("%d\n", i);
        for (j = 0; j < SIZE; j++) {
            address_c = (unsigned int)c + ((i * SIZE * sizeof(int))) + (j * sizeof(int));
            GetVal((void *)address_c, &y, sizeof(int));
            printf("%d ", y);
        }
        printf("\n");
    }
}

int main() {
    /*SetPhysicalMem();
    PageMap(NULL, (void *)0x00000000, 0x00050000);
    PageMap(NULL, (void *)0x00010000, 0x00060000);

    printf("address: %ld\n", getVA(80));

    pte_t* thing = Translate(NULL, (void *)0x00000000); //(pd == 0, pt == 00)
    pte_t* thing2 = Translate(NULL, (void *)0x00010000); //(pd == 0, pt == 16)

    printf("next Avalible 1 page VA: %ld\n", get_next_avail(15));
    printf("next Avalible 81 page VA: %ld\n", get_next_avail(81));
    

    if(thing == NULL && thing2 == NULL){
        printf("pog]\n"); return 0;
    }
    printf("Addres of thign: %ld\n", *thing);
    printf("Addres of thign2: %ld\n", *thing2);
*/
    //myalloc(1);
    pthread_t thread1, thread2, thread3, thread4;
    pthread_t thread11, thread21, thread31, thread41;
    pthread_create(&thread1, NULL, matMulT, NULL);
    pthread_create(&thread2, NULL, matMulT, NULL);
    // pthread_create(&thread3, NULL, matMulT, NULL);
    // pthread_create(&thread4, NULL, matMulT, NULL);
    // pthread_create(&thread11, NULL, matMulT, NULL);
    // pthread_create(&thread21, NULL, matMulT, NULL);
    // pthread_create(&thread31, NULL, matMulT, NULL);
    // pthread_create(&thread41, NULL, matMulT, NULL);

    unsigned long *mat1, *mat2, *mat3, *mat4;
    unsigned long *mat11, *mat21, *mat31, *mat41;
    struct thing ans;

    pthread_join(thread1, &mat1);
    pthread_join(thread2, &mat2);
    ans.a = mat1;
    ans.b = mat2;
    // pthread_join(thread3, &mat3);
    // pthread_join(thread4, &mat4);
    // pthread_join(thread11, &mat11);
    // pthread_join(thread21, &mat21);
    // pthread_join(thread31, &mat31);
    // pthread_join(thread41, &mat41);

    pthread_create(&thread3, NULL, matMulT2, (void*) &ans);
    pthread_create(&thread4, NULL, matMulT2, (void*) &ans);
    pthread_join(thread3, &mat3);
    pthread_join(thread4, &mat4);
    // printf("Address of Mat1: %x, ",mat1);
    // printf("Address of Mat2: %x/n",mat2);

    printMat(mat1);
    printf("\n\n");
    printMat(mat2);
    printf("\n\n");
    printMat(mat3);
    printf("\n\n");
    printMat(mat4);
    printf("\n\n");
    print_TLB_missrate();
    // printMat(mat11);
    // printf("\n\n");
    // printMat(mat21);
    // printf("\n\n");
    // printMat(mat31);
    // printf("\n\n");
    // printMat(mat41);

/*
    void* add1 = myalloc(30);
    void* add2 = myalloc(45);
    void* add3 = myalloc(10);
    void* add4 = myalloc(10);

    printf("Add1: %ld\n", Translate(NULL, add1));
    printf("Add1 Virt: %x\n", (int)add1);
    printf("Add2: %ld\n", Translate(NULL, add2));
    printf("Add2 Virt: %x\n", (int)add2);
    printf("Add3: %ld\n", Translate(NULL, add3));
    printf("Add3 Virt: %x\n", (int)add3);
    printf("Add4: %ld\n", Translate(NULL, add4));
    printf("Add4 Virt: %x\n", (int)add4);

    myfree(add2, 45);
    myfree(add1, 30);
    add1 = myalloc(10000);
    myfree(add4, 10);
    add2 = myalloc(1000);
    printf("Add1 again: %ld\n", Translate(NULL, add1));
    printf("Add1 Virt: %x\n", (int)add1);
    printf("Add2 agian: %ul\n", Translate(NULL, add2));
    printf("Add2 Virt %ul\n", add2);
*/
/*
    void* add = myalloc(sizeof(int)+1);
    int a = 5;
    void* thing = Translate(NULL, add+1);
    if(thing == NULL) printf("YEEERRRRR");
    else printf("Start, Request = %u, %u\n", (unsigned long)Translate(NULL,add), (unsigned long)thing);

    PutVal(add+1, &a, sizeof(int));
    printf("her\n");
    int b;
    GetVal(add+1, &b, sizeof(int));
    printf("%d\n", b);
    */
    /*
    printf("Starting...\n");
    void* a = myalloc(sizeof(int));
    int b = 5;
    PutVal(a, &b, sizeof(int));
    //void* c = myalloc(sizeof())
    printf("%d\n", *((int*) Translate(NULL, a))  );
    int d= 0;
    GetVal(a, &d, sizeof(int));
    printf("%d\n", d);*/
}