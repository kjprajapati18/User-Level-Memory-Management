#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../my_vm.h"


int main() {
    SetPhysicalMem();
    PageMap(NULL, (void *)0x00050000, 0x00050000);
    PageMap(NULL, (void *)0x00060000, 0x00050000);

    pte_t* thing = Translate(NULL, (void *)0x00050000); //(pd == 0, pt == 80)
    pte_t* thing2 = Translate(NULL, (void *)0x00060000); //(pd == 0, pt == 96)

    if(thing == NULL && thing2 == NULL){
        printf("pog]\n"); return;
    }
    printf("Addres of thign: %ld\n", thing);
    printf("Addres of thign2: %ld\n", thing2);
}