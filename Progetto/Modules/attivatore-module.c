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
    int id=semget(KEY_SEMAFORO, 0, 0666); //ottengo id del semaforo
    struct sembuf my_op ;
    my_op . sem_num = 0;//scelgo il semaforo di sincronizzazione
    my_op . sem_flg = 0;
    semop ( id , & my_op , 1) ;//eseguo le operazioni
    int flag=0;
    do
    {
        printf("Semaforo di sincronizzazione %d\n",semctl(id, 0, GETVAL, 0));
        if(semctl(id, 0, GETVAL, 0)==1) //se il semaforo è sincronizzato
        {
            printf("Semaforo prioritario %d\n",semctl(id, 1, GETVAL, 0));
            if(semctl(id, 1, GETVAL, 0)==1) //se il semaforo prioritario è libero
            {
                printf("Semafori liberi\n");
                flag=1;
                my_op . sem_num = 1;//scelgo il semaforo prioritario
                my_op . sem_flg = 0;
                my_op . sem_op = -1;//occupo il semaforo
                semop ( id , & my_op , 1) ;//eseguo le operazioni

                //sezione critica inizio
                struct memCond *p;
                int id=shmget(KEY_MEMORIA_CONDIVISA, sizeof(p), 0666); //ottengo id della memoria condivisa
                p=shmat(id, NULL, SHM_RDONLY); //ottengo il puntatore alla memoria condivisa
                int nAtomi = p->nAtomi;
                // int *vPid = p->vPid;
                //int pidVittima = vPid[rand()%nAtomi];
                //kill(pidVittima, SIGKILL);
                printf("Scelgo atomo vittima\n");

                //sezione critica fine

                my_op . sem_op = 1;//rilascio il semaforo
                semop ( id , & my_op , 1) ;//eseguo le operazioni
            }
        }
    } while (flag==0);
    
    
    
    
    
    
}