#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/**
 * Il metodo consiste nel scegliere in modo casuale uno dei N atomi da scindere e lo si fa ottenendo
 * innanzitutto il vettore dei pid condiviso e si genera un valore casuale che viene usato come indice per 
 * scegliere un atomo da scindere e infine si aggiornano le statisitche, precisamente nScissioni ed nScissioniAlSecondo
 * Per comunicare la scissione lo si fa attraverso il segnale SIGUSR2
*/
void scegliAtomoVittima();

/*Operazione sul semaforo per decrementare*/
void V(int nSem);

/*Operazione sul semaforo per incrementare*/
void P(int nSem);