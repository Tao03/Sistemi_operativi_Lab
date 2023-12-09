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
#include "../Headers/master.h"


void setSemaforo()
{
    key_t key = 1234;
    int id=semget(key, 3, IPC_CREAT);
    semctl(id, 0, SETVAL, 1);
    semctl(id, 1, SETVAL, 1);
    semctl(id, 2, SETVAL, 1);    
}

int setMemoriaCondivisa() // id = 32819
{
    key_t key= 1111;
    struct memCond
    {
        int * vPid; //vettore dei pid degli atomi
        int nAtomi; //grandezza del vettore
        int eTot;   //energia totale sprigionata

    }dummy; //è solo per la creazione della memoria condivisa
    int id = shmget(key, sizeof(dummy), IPC_CREAT | 0666);
    return id;
}
