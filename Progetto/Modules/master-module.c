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
#include "../Headers/risorse.h"

void setSemaforo()
{
    int id = semget(KEY_SEMAFORO, 3, 0); // Prova a ottenere l'ID del semaforo esistente

    if (id != -1) // Se l'ID non è -1, il semaforo esiste
    {
        // Rimuovi il semaforo esistente
        printf("Semaforo esistente trovato, rimozione...\n");
        if (semctl(id, 0, IPC_RMID) == -1)
        {
            perror("Errore nella rimozione del semaforo: ");
            exit(EXIT_FAILURE);
        }
    }

    // Crea un nuovo semaforo
    id = semget(KEY_SEMAFORO, 3, IPC_CREAT | IPC_EXCL | 0666);
    if (id == -1)
    {
        perror("Errore nella creazione del semaforo: ");
        exit(EXIT_FAILURE);
    }

    // Imposta i valori iniziali dei semafori
    semctl(id, 0, SETVAL, 1);/*-1 impostato a 1 solo per test*/
    semctl(id, 1, SETVAL, 1);
    semctl(id, 2, SETVAL, 1);
    //printf("Valore semaforo sincronizzazione: %d\n",semctl(id, 0, GETVAL, 0)); funziona
}

int setMemoriaCondivisa(int nKids) // id = 32819
{
    
    int id = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    /**
     * Si crea un array di interi condiviso
    */
    int id_array_condiviso = shmget(1222, sizeof(int) * nKids, IPC_CREAT | 0666);

    if (id == -1  || id_array_condiviso == -1)
    {
        perror("Errore nella creazione della memoria condivisa: ");
        exit(EXIT_FAILURE);
    }
    /**
     * Appena creata la memoria condivisa, nessun processo è in fase di scrittura quindi si potrebbe tranquillamente
     * entrare all'interno della risorsa senza concorrenza
    */
    struct memCond * datap ; /* shared data struct */
    int* new_array = (int*) shmat(id_array_condiviso, NULL, 0);

    datap = shmat ( id, NULL , 0) ;

    //datap->vPid = new_array;
    int new_shm_id = shmget(1222, datap->nAtomi, IPC_CREAT | 0666);
    datap->nAtomi = nKids;
    //memcpy(new_array, datap->vPid, datap->nAtomi * sizeof(int));
    //int new_shm_id = shmget(1222, datap->nAtomi, IPC_CREAT | 0666);
    datap->id_vettore_condiviso = new_shm_id;
    int status = shmdt (datap);
    if(status == -1){
        perror("Error: ");
    }
    printf("CHECKPOINT: MEMORIA CONDIVISA CREATA CON SUCCESSO!\n");
    return id;
}

void creaAtomi(int nAtomi, int nAtomoMax, int idMemoriaCondivisa)
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
            char  * const array[2] = {stringa,0};
            execv("Atomo",array);
            perror("");
            exit(1);
        }else{
            /**
             * Porzione di codice che consiste nel aggiungere il pid degli atomi in memoria condivisa
            */
           insertAtomi(i,pid,idMemoriaCondivisa);
        }
        
    }
    //printf("CHECKPOINT: la creazione degli atomi è andata a buon fine!\n");
    /**
     * Fino a qui, il codice funziona correttamente
    */

    
}
void insertAtomi(int indice, int pid, int idMemoriaCondivisa){
    struct sembuf my_op ;
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    semop ( 1234 , & my_op , 1) ; /* blocking if others hold resource */
    printf("CHECKPOINT: Accesso alla memoria condivisa!\n");
    //printf("ID dell'array di semafori: %d, valore semaforo %d\n",id, semctl(id, 0, GETVAL));
    struct memCond * datap ; /* shared data struct */
    datap = shmat ( idMemoriaCondivisa, NULL , 0) ;
    if (datap == (struct memCond *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    //datap->vPid[0] = pid;
    /**
     * QUI SI DEVE INSERIRE LA PARTE DI CODICE CHE INSERISCE I PROCESSI
    */
    printf("Numero atomi: %d\n",datap->nAtomi);
    /**
     * 
     * 
     * 
     * SEZIONE CRITICA
     * 
     * 
     * 
     * 
    */
   my_op . sem_op = 1; /* releasing the resource */
   semop ( 1234 , & my_op , 1) ; /* may un - block others */
   printf("CHECKPOINT: Rilascio memoria condivisa\n\n");
}

