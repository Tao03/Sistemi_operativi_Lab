#define _GNU_SOURCE 
#include "../Headers/alimentatore.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "../Headers/risorse.h"

void creaAtomi(int nAtomi){
    
    /**
     * Dobbiamo innanzitutto allargare la dimensione del vettorei dei pid processi atomo
    */

    for(int i=0;i<nAtomi;i++){
        int pid = fork();
        if(pid == 0){
            /**
             * Cambio immagine processo
            */
            printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
            srand(getpid());
            /**
             * PROBLEMA: siccome non abbiamo la macro del numero atomico, per ora utilizzeremo una costante
            */
            int numeroAtomico = rand()%100;
            char stringa[100];
            sprintf(stringa, "%d", numeroAtomico);
            char  * const array[2] = {stringa,0};
            execv("Atomo",array);
            perror("");
            exit(1);
        }else{
            printf("CASSA\n");
            aggiungiProcessoAtomo(pid);
        }
    }
    
}
/**
     * Modifica il numero dei processi atomi in esecuzione
*/
void aggiungiProcessoAtomo(int pid){
    
    struct sembuf my_op ;
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    semop ( 1234 , & my_op , 1) ; /* blocking if others hold resource */
    printf("CHECKPOINT: Accesso alla memoria condivisa!\n");
    struct memCond * datap ; /* shared data struct */
    int idMemoriaCondivisa = shmget(1111,sizeof(datap),0);
    datap = shmat ( idMemoriaCondivisa, NULL , 0) ;

    if (datap == (struct memCond *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    datap->nAtomi = datap->nAtomi + 1;
     /**
     * Ri-allocamento del vettore con dimensione n + 1 dove n è la dimensione prima dell'esecuzione del metodo
    */
    realloc( datap->vPid,sizeof(int)*datap->nAtomi);
    printf("Numero atomi nuovo è: : %d\n",datap->nAtomi);
    /**
     * Aggiunta del pid nel vettore
    */
    datap->vPid[datap->nAtomi-1]=pid;
    my_op . sem_op = 1; /* releasing the resource */
    semop ( 1234 , & my_op , 1) ; /* may un - block others */
}