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
#define KEY_SEMAFORO 1234
#define KEY_MEMORIA_CONDIVISA 1111

void setSemaforo()
{
    int id = semget(KEY_SEMAFORO, 3, IPC_CREAT);
    if (id == -1)
    {
        perror("Errore nella creazione del semaforo: ");
        exit(EXIT_FAILURE);
    }
    semctl(id, 0, SETVAL, 1);
    semctl(id, 1, SETVAL, 1);
    semctl(id, 2, SETVAL, 1);
}

int setMemoriaCondivisa() // id = 32819
{
    struct memCond
    {
        int *vPid;  // vettore dei pid degli atomi
        int nAtomi; // grandezza del vettore
        int eTot;   // energia totale sprigionata

    } dummy; // è solo per la creazione della memoria condivisa
    int id = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    if (id == -1)
    {
        perror("Errore nella creazione della memoria condivisa: ");
        exit(EXIT_FAILURE);
    }
    //printf("CHECKPOINT: MEMORIA CONDIVISA CREATA CON SUCCESSO!\n");
    return id;
}

void creaAtomi(int nAtomi, int nAtomoMax)
{
    for (int i = 0; i < nAtomi; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
            srand(getpid());
            int numeroAtomico = rand()%nAtomoMax;
            char stringa[100];
            sprintf(stringa, "%d", numeroAtomico);
            printf("Valore atomico: %s",stringa);
            char  * const array[2] = {stringa,0};
            execv("Atomo",array);
            perror("");
            exit(1);
        }
        
    }
    printf("CHECKPOINT: la creazione degli atomi è andata a buon fine!\n");
    
}