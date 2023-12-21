#include "Headers/master.h"
#include <stdio.h>
#include "Headers/risorse.h"
void main()
{
    /**
     * Inizializzazione dei 3 semafori
    */
    setSemaforo();
    printf("Semafori settati\n");
    /**
     * Inizializzazione della memoria condivisa
    */
    int idMemoriaCondivisa=setMemoriaCondivisa(N_ATOMI_INIT);
    printf("Memoria condivisa creata, id memoria: %d\n",idMemoriaCondivisa);

    /**
     * Creazione processi atomi
    */
   creaAtomi(N_ATOMI_INIT,N_ATOMO_MAX,idMemoriaCondivisa);
   
}