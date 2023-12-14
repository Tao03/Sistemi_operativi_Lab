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
    key_t key= 1234; //chiave del semaforo
    int id=semget(key, 1, 0666); //ottengo id del semaforo
    struct sembuf my_op ;
    my_op . sem_num = 0;//scelgo il semaforo di sincronizzazione
    my_op . sem_flg = 0;
    my_op . sem_op = 0;//leggo il semaforo
    semop ( id , & my_op , 1) ;//eseguo le operazioni
    int flag=0;
    do
    {
        printf("Entro nella roba\n");
        if(semctl(id, 0, GETVAL, 0)==1) //se il semaforo è sincronizzato
        {
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
                int *vPid = p->vPid;
                int pidVittima = vPid[rand()%nAtomi];
                kill(pidVittima, SIGUSR1);
                //sezione critica fine

                my_op . sem_op = 1;//rilascio il semaforo
                semop ( id , & my_op , 1) ;//eseguo le operazioni
            }
        }
    } while (flag==0);
    
    
    
    
    
    
}