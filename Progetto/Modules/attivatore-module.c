#define _GNU_SOURCE
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "../Headers/risorse.h"
#include "../Headers/attivatore.h"
void handler(int signal)
{
    printf("Sono entrato nell'handler\n");
}
void scegliAtomoVittima()
{
    int idsem=semget(KEY_SEMAFORO, 0, 0666); //ottengo id del semaforo
    struct sembuf my_op;
    my_op . sem_num = 1;//scelgo il semaforo prioritario
    my_op . sem_flg = 0;
    my_op . sem_op = -1;//occupo il semaforo
    semop ( idsem, & my_op , 1) ;//eseguo le operazioni
    //printf("SEMAFORO LIBERO PER ATTIVATORE\n");
    struct sigaction sa;
    sigset_t my_mask;
    sa.sa_handler=&handler;


    //sezione critica inizio
        struct memCond *p;
        int idmem=shmget(KEY_MEMORIA_CONDIVISA, sizeof(p), 0666); //ottengo id della memoria condivisa
        p=shmat(idmem, NULL, 0); //ottengo il puntatore alla memoria condivisa
        //printf("ACCESSO MEMORIA CONDIVISA\n");
        int nAtomi = p->nAtomi;
        //printf("ACCESSO VETTORE CONDIVISO\n");
        int *vPid = shmat(p->id_vettore_condiviso, NULL, 0); //ottengo il puntatore al vettore
        
        /**
         * E' sconveniente usare getPid() perchè srand usa sempre lo stesso valore(overro il pid dell'allimentatore siccome muore alla fine della
         * simulazione) e quindi genera lo stesso valore casuale
        */
        srand(time(NULL));
        int indiceProcessoVittima = rand()%nAtomi;
        int pidVittima = vPid[indiceProcessoVittima];
        if(vPid[indiceProcessoVittima] != -1){
            kill(pidVittima, SIGUSR1);
            printf("SCISSIONE FATTA %d\n",pidVittima);
            p->nAttivazioni++; 
        }
        shmdt(p);

    //sezione critica fine
    my_op . sem_num = 1;//scelgo il semaforo prioritario
    my_op . sem_flg = 0;
    my_op . sem_op = 1;//rilascio il semaforo
    semop ( idsem , & my_op , 1) ;//eseguo le operazioni
    
    
    
    
    
    
}