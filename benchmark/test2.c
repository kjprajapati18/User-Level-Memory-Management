#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../my_vm.h"


int main() {
    SetPhysicalMem();
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
}