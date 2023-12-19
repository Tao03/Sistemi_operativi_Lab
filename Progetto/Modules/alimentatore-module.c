#define _GNU_SOURCE
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
#include "../Headers/alimentatore.h"
#include "../Headers/risorse.h"

void creaAtomi(int nAtomi){
    
    /**
     * Dobbiamo innanzitutto allargare la dimensione del vettorei dei pid processi atomo
    */
   printf("creaAtomi è stato invocato\n");
    for(int i=0;i<2;i++){
        int pid = fork();
        if(pid == 0){
            /**
             * Cambio immagine processo
            */
           // printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
            srand(getpid());
            /**
             * PROBLEMA: siccome non abbiamo la macro del numero atomico, per ora utilizzeremo una costante
            */
            int numeroAtomico = rand()%100;
            char stringa[100] = "    ";
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
    datap = (struct memCond *) shmat ( idMemoriaCondivisa, NULL , 0) ;

    if (datap->vPid == (int *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    
    datap->nAtomi = datap->nAtomi + 1;
    printf("CHECKPOINT: il numero di atomi è stato incrementato a %d\n",datap->nAtomi);
     /**
     * Ri-allocamento del vettore con dimensione n + 1 dove n è la dimensione prima dell'esecuzione del metodo
    */
   int* array = datap->vPid;
   if(array == NULL){
        printf("Errore in memoria condivisa\n");
        exit(EXIT_FAILURE);
   }
   printf("Prima di realloc, array: %p\n", (void *)array);


    /*
    array =  (int*) realloc( (void *)array,sizeof(int)*(10));
    */

    printf("Prima di realloc, array: %p\n", (void *)array);
    if ((void *)array == NULL) {
        printf("Errore di riallocazione di memoria\n");
        exit(EXIT_FAILURE);
    }
    printf("CHECKPOINT: Numero atomi nuovo è: : %d\n",datap->nAtomi);
    /**
     * Aggiunta del pid nel vettore
    */
   /**
    * DA QUELLO CHE HO CAPITO, facendo array[0] = pid mi da errore MA FORSE anche realloc()
   */
    //array[0] = pid;
    datap->vPid[0]= pid;

    printf("CHECKPOINT: Atomo inserito nel vettore : %d\n",datap->nAtomi);
    if (shmdt(datap) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    my_op . sem_op = 1; /* releasing the resource */
    semop ( 1234 , & my_op , 1) ; /* may un - block others */
    printf("CHECKPOINT: Scollegamento dalla memoria!\n");
}
/**
 * E' Presente un problema: 
 *      Siccome l'array è stato inizializzato dal master, all'interno della struct mettiamo soltanto il puntatore
 *      ma il vettore effettivo è all'interno dell'area privata del processo master e quindi l'alimentatore avendo il 
 *      puntatore all'array non riesce ad accedere.
*/