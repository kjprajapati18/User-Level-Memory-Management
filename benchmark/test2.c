#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../my_vm.h"


int main() {
    printf("%d\n", (int) pow(2,4));
    SetPhysicalMem();
    Translate(NULL, (void *)0x00050000); //(pd == 0, pt == 80)
}