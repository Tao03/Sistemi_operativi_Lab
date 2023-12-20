#include "Headers/master.h"
#include <stdio.h>
#include "Headers/risorse.h"
void main()
{
    



    /**
     * Inizializzazione dei 3 semafori
    */
    struct sembuf my_op ;
    int idSemaforo = setSemaforo();
    my_op . sem_num = 2; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
   printf("Ho appena dato il via!\n");
   semop ( idSemaforo , & my_op , 1) ; 
    /**
     * Inizializzazione della memoria condivisa
    */
    int idMemoriaCondivisa=setMemoriaCondivisa(N_ATOMI_INIT);
    printf("id della memoria condivisa: %d\n",idMemoriaCondivisa);

    /**
     * Creazione processi atomi
    */
   creaAtomi(N_ATOMI_INIT,N_ATOMO_MAX,idMemoriaCondivisa);

   sleep(10);
  
    my_op . sem_num = 2; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = 1; /* accessing the resource */
   printf("Ho appena dato il via!\n");
   semop ( idSemaforo , & my_op , 1) ; 
   
}