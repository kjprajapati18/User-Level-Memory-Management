#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../my_vm.h"


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