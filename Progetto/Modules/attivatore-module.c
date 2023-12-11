#include "Progetto/Headers/attivatore.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "../Headers/risorse.h"
void scegliAtomoVittima()
{
    key_t key= 1234; //chiave del semaforo
    int id=semget(key, 1, 0666); //ottengo id del semaforo
    struct sembuf my_op ;
    my_op . sem_num = 0;//scelgo il semaforo di sincronizzazione
    my_op . sem_flg = 0;
    my_op . sem_op = 0;//leggo il semaforo
    semop ( id , & my_op , 1) ;//eseguo le operazioni
    if(semctl(id, 0, GETVAL, 0)==1) //se il semaforo è sincronizzato
    {
        my_op . sem_num = 1;//scelgo il semaforo prioritario
        my_op . sem_flg = 0;
        my_op . sem_op = -1;//occupo il semaforo
        semop ( id , & my_op , 1) ;//eseguo le operazioni
        if (semctl(id, 1, GETVAL, 0)==1) //se il semaforo prioritario è libero
        {
            
            /**
             * 
             * 
             * 
             * SEZIONE CRITICA, 
             * 
             * 
             * 
             * 
            */
            my_op . sem_op = 1;//rilascio il semaforo
            semop ( id , & my_op , 1) ;//eseguo le operazioni
    }
    }
    
    
    
    
}