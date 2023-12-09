#include "Headers/master.h"
#include <stdio.h>
void main()
{
    setSemaforo();
    printf("Ciao!\n");
    int id=setMemoriaCondivisa();
    printf("id della memoria condivisa: %d\n",id);
}