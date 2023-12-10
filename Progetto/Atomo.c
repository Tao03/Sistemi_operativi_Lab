#include <stdio.h>
#include <stdlib.h>
#include "Headers/atomo.h"
int main(int argc, char* argv[]){
    printf("Sono il processo atomo e sono stato eseguito!\n");
    int val = strtol(argv[0],NULL,10);
    printf("Il mio numero atomico è: %d \n",val);
}