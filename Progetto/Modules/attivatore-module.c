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
int idSemaforo;
struct sembuf my_op;
void scegliAtomoVittima()
{
    idSemaforo = semget(KEY_SEMAFORO, 0, 0666); // ottengo id del semaforo
    V(1);
    // sezione critica inizio
    struct memCond *p;
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(p), 0666); // ottengo id della memoria condivisa
    p = shmat(idMemoriaCondivisa, NULL, 0);                                  // ottengo il puntatore alla memoria condivisa
    // printf("ACCESSO MEMORIA CONDIVISA\n");
    int nAtomi = p->nAtomi;
    // printf("ACCESSO VETTORE CONDIVISO\n");
    int *vPid = shmat(p->id_vettore_condiviso, NULL, 0); // ottengo il puntatore al vettore

    /**
     * E' sconveniente usare getPid() perchè srand usa sempre lo stesso valore(overro il pid dell'allimentatore siccome muore alla fine della
     * simulazione) e quindi genera lo stesso valore casuale
     */
    srand(time(NULL));
    int indiceProcessoVittima = rand() % nAtomi;
    int pidVittima = vPid[indiceProcessoVittima];
    if (vPid[indiceProcessoVittima] != -1)
    {
        kill(pidVittima, SIGUSR2);
        p->nAttivazioni = p->nAttivazioni + 1;
        p->nAttivazioniUltimoSecondo = p->nAttivazioniUltimoSecondo + 1;
    }
    shmdt(p);
    shmdt(vPid);

    // sezione critica fine
    P(1);
}
/*Operazione sul semaforo per decrementare*/
void P(int nSem)
{
    idSemaforo  = semget(KEY_SEMAFORO, 0, 0);
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0;    /* no flag : default behavior */
    my_op.sem_op = 1;     /* accessing the resource */
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore sul semaforo: ");
    }
}
/*Operazione sul semaforo per incrementare*/
void V(int nSem)
{
    idSemaforo  = semget(KEY_SEMAFORO, 0, 0);
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0;    /* no flag : default behavior */
    my_op.sem_op = -1;    /* accessing the resource */
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore sul semaforo: ");
    }
}