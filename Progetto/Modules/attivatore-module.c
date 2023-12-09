#include "Progetto/Headers/attivatore.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
void scegliAtomoVittima()
{
    key_t key= 1234; //chiave del semaforo
    int id=semget(key, 1, 0666); //ottengo id del semaforo
    struct sembuf my_op ;
    my_op . sem_num = 1;//scelgo il semaforo prioritario
    my_op . sem_flg = 0;
    my_op . sem_op = 0;//leggo il semaforo
    if (semop(id,&my_op, 1))
    {
        my_op . sem_num = 1;//scelgo il semaforo prioritario
        my_op . sem_flg = 0;
        my_op . sem_op = -1;//occupo il semaforo
        semop ( id , & my_op , 1) ;//eseguo le operazioni
        /**
         * 
         * 
         * 
         * SEZIONE CRITICA, c'è ancora da settare la mem. condivisa
         * 
         * 
         * 
         * 
        */
        my_op . sem_op = 1;//rilascio il semaforo
        semop ( id , & my_op , 1) ;//eseguo le operazioni
    }
    
    
}