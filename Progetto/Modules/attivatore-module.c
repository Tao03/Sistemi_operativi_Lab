#define _GNU_SOURCE
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "../Headers/risorse.h"
#include "../Headers/attivatore.h"
void scegliAtomoVittima()
{
    int idsem=semget(KEY_SEMAFORO, 0, 0666); //ottengo id del semaforo
    printf("\n->Sono attivatore e scelgo atomo vittima\n");
    struct sembuf my_op;
    my_op . sem_num = 1;//scelgo il semaforo prioritario
    my_op . sem_flg = 0;
    my_op . sem_op = -1;//occupo il semaforo
    semop ( idsem, & my_op , 1) ;//eseguo le operazioni

    //sezione critica inizio
        struct memCond *p;
        int idmem=shmget(KEY_MEMORIA_CONDIVISA, sizeof(p), 0666); //ottengo id della memoria condivisa
        p=shmat(idmem, NULL, SHM_RDONLY); //ottengo il puntatore alla memoria condivisa
        //printf("ACCESSO MEMORIA CONDIVISA\n");
        int nAtomi = p->nAtomi;
        //int idvet=shmget(KEY_ARRAY_CONDIVISO, sizeof(int)*nAtomi, 0666); //ottengo id del vettore
        //printf("ACCESSO VETTORE CONDIVISO\n");
        int *vPid = shmat(p->id_vettore_condiviso, NULL, SHM_RDONLY); //ottengo il puntatore al vettore
        
        printf("Numero atomi: %d\n",nAtomi);
        srand(getpid());
        int pidVittima = vPid[rand()%nAtomi];
        kill(pidVittima, SIGKILL);
        printf("Atomo ucciso %d\n",pidVittima);

    //sezione critica fine
    my_op . sem_num = 1;//scelgo il semaforo prioritario
    my_op . sem_flg = 0;
    my_op . sem_op = 1;//rilascio il semaforo
    semop ( idsem , & my_op , 1) ;//eseguo le operazioni
    printf("->Esco da scegliAtomoVittima\n\n");
    
    
    
    
    
    
}