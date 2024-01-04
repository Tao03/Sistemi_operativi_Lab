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





#include "../Headers/alimentatore.h"
#include "../Headers/risorse.h"
int id = 0;
void creaAtomi()
{

    /**
     * Dobbiamo innanzitutto allargare la dimensione del vettorei dei pid processi atomo
     */
    // printf("ciaoo\n");
     char pidMaster[100];
    sprintf(pidMaster, "%d", getppid()); // converto il numero atomico in stringa (per passarlo come parametro)
    struct sembuf my_op;
    my_op.sem_num = 1; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1; /* accessing the resource */
    int idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
    if (idSemaforo == -1)
    {
        perror("Errore sul semaforo: ");
    }
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        perror("Semaforo 2 semop");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < N_NUOVI_ATOMI; i++)
    {
        printf("ATOMO AGGIUNTO\n");
        int pid = fork();
        if (pid == 0)
        {
            /**
             * Cambio immagine processo
             */
            // printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
            srand(getpid());
            /**
             * PROBLEMA: siccome non abbiamo la macro del numero atomico, per ora utilizzeremo una costante
             */
            int numeroAtomico = rand() % 100;
            char stringa[100] = "    ";
            sprintf(stringa, "%d", numeroAtomico);
           
            char *const array[3] = {stringa,pidMaster, 0};
            execv("Atomo", array);
            perror("");
            kill(strtol(pidMaster,NULL,10),SIGUSR2);
            exit(1);
        }
        else
        {
            aggiungiProcessoAtomo(pid);
        }
    }
    my_op.sem_op = 1; /* releasing the resource */
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        perror("Semaforo 0 semop");
        exit(EXIT_FAILURE);
    }
}
/**
 * Modifica il numero dei processi atomi in esecuzione
 */
void aggiungiProcessoAtomo(int pid)
{

    

    // printf("CHECKPOINT: Il processo alimentatore ha fatto l'accesso alla memoria condivisa!\n");
    struct memCond *datap; /* shared data struct */
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(datap), IPC_CREAT | 0666);
    if (idMemoriaCondivisa == -1)
    {
        perror("Error in shmeg ");
        exit(EXIT_FAILURE);
    }
    datap = (struct memCond *)shmat(idMemoriaCondivisa, NULL, 0);
    if (datap == NULL)
    {
        perror("Errore in shmat ");
        exit(EXIT_FAILURE);
    }


    add_int_to_shared_array(datap, pid);
    
    if (shmdt(datap) == -1)
    {
        perror("Errore in alimentatore per chiudere la memoria condivisa ");
        exit(EXIT_FAILURE);
    }
}


void add_int_to_shared_array(struct memCond *shared_struct, int pid)
{
    int old_shm_id = shared_struct->id_vettore_condiviso;

    // Recupera array condiviso:

    int *old_array = (int *)shmat(old_shm_id, NULL, 0);
    if (old_array == NULL)
    {
        perror("Error: %d");
    }

    
    int index = check(old_array, shared_struct->nAtomi);
    if (index != -1)
    {
        old_array[index] = pid;
        if (shmdt(old_array) == -1)
        {
            printf("shmdt\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int *private_array = malloc(sizeof(int) * shared_struct->nAtomi);
        memcpy(private_array, old_array, sizeof(int) * (shared_struct->nAtomi));
        /*for(int i = 0; i<shared_struct->nAtomi;i++){
            private_array[i] = old_array[i];
        }*/
        // printf("Puntatore del vecchio array: %p\n",old_array);
        // printf("Puntatore dell' array privato: %p\n",private_array);
        //  Elimina l'array condiviso
        if (shmdt(old_array) == -1)
        {
            printf("shmdt\n");
            exit(EXIT_FAILURE);
        }
        if (shmctl(old_shm_id, IPC_RMID, NULL) == -1)
        {
            printf("Errore di deallocazione nell'alimentatore del vettore condiviso \n");
            exit(EXIT_FAILURE);
        }

        shared_struct->nAtomi = shared_struct->nAtomi + 1;

        // Creazione dell'array condiviso di dimensione n + 1

        int new_shm_id = shmget(KEY_ARRAY_CONDIVISO, shared_struct->nAtomi, IPC_CREAT | 0666);

        if (new_shm_id == -1)
        {
            perror("Errore della creazione dell'array di interi  ");
        }
        // Aggiornamento id nuovo array con dimensione ( n + 1 )

        shared_struct->id_vettore_condiviso = new_shm_id;

        // int* temp_array = malloc(sizeof(int)*(shared_struct->nAtomi));

        int *new_array = (int *)shmat(new_shm_id, NULL, 0);
        // printf("Primo processo: %d",new_array[0]);
        if (new_array == NULL)
        {

            // printf("Errore nel collegamento della nuova memoria condivisa \n");

            exit(EXIT_FAILURE);
        }

        /*if (shmdt(shared_struct->vPid) == -1) {
            perror("Errore nello staccare il segmento di memoria condivisa");
            // Puoi gestire l'errore in modo appropriato
        }*/
        /*for(int i = 0; i<shared_struct->nAtomi-1;i++){
            new_array[i] = private_array[i];
        }*/

        // Copia i dati dal vecchio vettore al nuovo
        memcpy(new_array, private_array, (shared_struct->nAtomi - 1) * sizeof(int));
        // Aggiungi il nuovo intero al vettore

        new_array[shared_struct->nAtomi - 1] = pid;

        // printf("Pid del primo processo: %d\n",new_array[0]);

        // Scollega e rilascia il vecchio segmento di memoria condivisa per il vettore

        // shmdt(new_array);

        // Aggiorna la struct in memoria condivisa per usare il nuovo vettore

        // shared_struct->vPid = new_array;

        /*
        shmdt(new_array);
        shmctl ( new_shm_id , 0 , NULL ) ;
        */

        if (shmdt(new_array) == -1)
        {
            perror("Errore nella chiusura dell'array condiviso nell'alimentatore: ");
            exit(EXIT_FAILURE);
        }
    }
}
int check(int *array, int size)
{
    int index = -1;
    for (int i = 0; i < size && index == -1; i++)
    {
        if (*(array + i) == -1)
        {
            index = i;
        }
    }
    return index;
}