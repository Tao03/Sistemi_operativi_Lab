#include "Headers/master.h"
#include <stdio.h>
#define N_ATOMI_INIT  2
#define N_ATOMO_MAX 100
void main()
{
    /**
     * Inizializzazione dei 3 semafori
    */
    setSemaforo();
    printf("Ciao!\n");
    /**
     * Inizializzazione della memoria condivisa
    */
    int idMemoriaCondivisa=setMemoriaCondivisa(N_ATOMI_INIT);
    printf("id della memoria condivisa: %d\n",idMemoriaCondivisa);

    /**
     * Creazione processi atomi
    */
   creaAtomi(N_ATOMI_INIT,N_ATOMO_MAX,idMemoriaCondivisa);
   
}