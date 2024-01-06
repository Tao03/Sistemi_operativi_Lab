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
int idSemaforo;
struct sembuf my_op;

/*Operazione sul semaforo per decrementare*/
void P(int nSem)
{
    idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
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
    idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0;    /* no flag : default behavior */
    my_op.sem_op = -1;    /* accessing the resource */
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore sul semaforo: ");
    }
}

void creaAtomi()
{
    idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
    if (idSemaforo == -1)
    {
        fprintf(stderr, "Errore nel ottenere l'identificatore del vettore di semafori , linea %d", __LINE__);
    }

    char pidMaster[100];
    sprintf(pidMaster, "%d", getppid()); // converto il numero atomico in stringa (per passarlo come parametro)
    V(1);
    for (int i = 0; i < N_NUOVI_ATOMI; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            srand(getpid());
            int numeroAtomico = rand() % N_ATOMICO_MAX;
            char stringa[100] = "    ";
            sprintf(stringa, "%d", numeroAtomico);
            char *const array[3] = {stringa, pidMaster, 0};
            execv("Atomo", array);
            fprintf(stderr, "Errore nel creare il processo atomo, linea %d, errore %d", __LINE__, errno);
            exit(1);
        }
        else if (pid == -1)
        {
            fprintf(stderr, "Errore nella creazione del processo atomo, linea %d errore %d \n", __LINE__, errno);
            kill(strtol(pidMaster, NULL, 10), SIGUSR2);
            exit(EXIT_FAILURE);
        }
        else
        {
            aggiungiProcessoAtomo(pid);
        }
    }
    P(1);
}
/**
 * Modifica il numero dei processi atomi in esecuzione
 */
void aggiungiProcessoAtomo(int pid)
{

    struct memCond *datap;
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(datap), IPC_CREAT | 0666);
    if (idMemoriaCondivisa == -1)
    {
        fprintf(stderr, "Errore nel ottenere l'identificatore della memoria condivisa nel processo alimentatore, linea %d errore %d \n", __LINE__, errno);
        exit(EXIT_FAILURE);
    }
    datap = (struct memCond *)shmat(idMemoriaCondivisa, NULL, 0);
    if (datap == NULL)
    {
        fprintf(stderr, "Errore nel collegare la memoria condivisa nel processo alimentatore, linea %d errore %d \n", __LINE__, errno);
        exit(EXIT_FAILURE);
    }

    add_int_to_shared_array(datap, pid);

    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Errore nel scollegarsi dal vettore condiviso , linea %d, errore %d\n", __LINE__, errno);
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
        fprintf(stderr, "Errore nel ottenere il vettore condiviso , linea %d, errore%d\n", __LINE__, errno);
        exit(EXIT_FAILURE);
    }
    //Si controlla se all'interno del vettore dei pid c'è una cella libera attraverso il metodo check
    int index = check(old_array, shared_struct->nAtomi);
    if (index != -1)
    {
        old_array[index] = pid;
        if (shmdt(old_array) == -1)
        {
            fprintf(stderr, "Errore nello scollegamento dal vettore condiviso nel processo alimentatore , linea %d, errore %d\n", __LINE__, errno);
            exit(EXIT_FAILURE);
        }
    }
    else
    /**Arrivando a questo ramo del costrutto if significa che non c'è nessuna cella all'interno della memoria condivisa libera
    quindi si deve allargare la memoria    
    */
    {
        
        int *private_array = malloc(sizeof(int) * shared_struct->nAtomi);

        // Si carica in un array locale gli elementi salvati in un array condiviso
        memcpy(private_array, old_array, sizeof(int) * (shared_struct->nAtomi));

        // Scollegamento del vecchio array
        if (shmdt(old_array) == -1)
        {
            fprintf(stderr, "Errore nel scollegarsi dal vettore condiviso , linea %d, errore %d\n", __LINE__, errno);
            exit(EXIT_FAILURE);
        }
        if (shmctl(old_shm_id, IPC_RMID, NULL) == -1)
        {
            fprintf(stderr, "Errore nel deallocare il vettore condiviso , linea %d, errore %d\n", __LINE__, errno);
            exit(EXIT_FAILURE);
        }

        //Si aggiorna il numero di atomi esistenti
        shared_struct->nAtomi = shared_struct->nAtomi + 1;

        // Creazione dell'array condiviso di dimensione n + 1

        int new_shm_id = shmget(KEY_ARRAY_CONDIVISO, shared_struct->nAtomi, IPC_CREAT | 0666);

        if (new_shm_id == -1)
        {
            fprintf(stderr, "Errore nel creare il vettore condiviso , linea %d, errore %d\n", __LINE__, errno);
        }
        // Aggiornamento id nuovo array con dimensione ( n + 1 )

        shared_struct->id_vettore_condiviso = new_shm_id;

        // int* temp_array = malloc(sizeof(int)*(shared_struct->nAtomi));

        int *new_array = (int *)shmat(new_shm_id, NULL, 0);
        if (new_array == NULL)
        {

            fprintf(stderr, "Errore nel collegarsi al vettore condiviso , linea %d, errore %d\n", __LINE__, errno);

            exit(EXIT_FAILURE);
        }

        // Copia i dati dal vecchio vettore al nuovo
        memcpy(new_array, private_array, (shared_struct->nAtomi - 1) * sizeof(int));

        
        if (new_array == NULL)
        {
            fprintf(stderr, "Errore nella copia del vecchio array attraverso il metodo memcpy(), linea %d, errore %d", __LINE__, errno);
        }
        else
        {
            // Aggiungi il nuovo intero al vettore
            new_array[shared_struct->nAtomi - 1] = pid;

            
        }
        // Scollegamento dell'array condiviso
        if (shmdt(new_array) == -1)
            {
                fprintf(stderr, "Errore nel scollegarsi dal vettore condiviso , linea %d", __LINE__);
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