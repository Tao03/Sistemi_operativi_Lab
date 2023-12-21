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
int id=0;
void creaAtomi(int nAtomi){
    
    /**
     * Dobbiamo innanzitutto allargare la dimensione del vettorei dei pid processi atomo
    */
   printf("ciaoo\n");
    for(int i=0;i<1;i++){
        int pid = fork();
        if(pid == 0){
            /**
             * Cambio immagine processo
            */
           // printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
            srand(getpid());
            /**
             * PROBLEMA: siccome non abbiamo la macro del numero atomico, per ora utilizzeremo una costante
            */
            int numeroAtomico = rand()%100;
            char stringa[100] = "    ";
            sprintf(stringa, "%d", numeroAtomico);
            char  * const array[2] = {stringa,0};
            execv("Atomo",array);
            perror("");
            exit(1);
        }else{
            //printf("CASSA\n");
            aggiungiProcessoAtomo(pid);
            //printf("Processo atomo con pid %d aggiunto \n",pid);
        }
    }
    
}
/**
     * Modifica il numero dei processi atomi in esecuzione
*/
void aggiungiProcessoAtomo(int pid){
    
    struct sembuf my_op ;
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    int idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
    if(idSemaforo == -1){
        perror("Errore sul semaforo: ");
    }
    if (semop(idSemaforo, &my_op, 1) == -1) {
        perror("Semaforo 2 semop");
        exit(EXIT_FAILURE);
    }

    printf("CHECKPOINT: Il processo alimentatore ha fatto l'accesso alla memoria condivisa!\n");
    struct memCond * datap ; /* shared data struct */
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA,sizeof(datap),IPC_CREAT | 0666);
    if(idMemoriaCondivisa == -1){
        perror("Error in shmeg ");
        exit(EXIT_FAILURE);
    }
    datap = (struct memCond *) shmat ( idMemoriaCondivisa, NULL , 0) ;
    if(datap == NULL){
        perror("Errore in shmat ");
        exit(EXIT_FAILURE);
    }

  /*  if (datap->vPid == (int *)(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }*/
    
    
    //datap->nAtomi = datap->nAtomi + 1;
    //printf("CHECKPOINT: il numero di atomi è stato incrementato a %d\n",datap->nAtomi);
     /**
     * Ri-allocamento del vettore con dimensione n + 1 dove n è la dimensione prima dell'esecuzione del metodo
    */
   /*int* array = datap->vPid;
   if(array == NULL){
        printf("Errore in memoria condivisa\n");
        exit(EXIT_FAILURE);
   }
   printf("Prima di realloc, array: %p\n", (void *)array);


    /*
    array =  (int*) realloc( (void *)array,sizeof(int)*(10));
    */

    /*printf("Prima di realloc, array: %p\n", (void *)array);
    if ((void *)array == NULL) {
        printf("Errore di riallocazione di memoria\n");
        exit(EXIT_FAILURE);
    }
    printf("CHECKPOINT: Numero atomi nuovo è: : %d\n",datap->nAtomi);*/
    /**
     * Aggiunta del pid nel vettore
    */
   /**
    * DA QUELLO CHE HO CAPITO, facendo array[0] = pid mi da errore MA FORSE anche realloc()
   
    //array[0] = pid;
    datap->vPid[0]= pid;

    printf("CHECKPOINT: Atomo inserito nel vettore : %d\n",datap->nAtomi);
    if (shmdt(datap) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    } */

    add_int_to_shared_array(datap,pid);

   // shmdt(datap);
    my_op . sem_op = 1; /* releasing the resource */
    if (semop(idSemaforo, &my_op, 1) == -1) {
        perror("Semaforo 0 semop");
        exit(EXIT_FAILURE);
    }
    if(shmdt(datap)==-1){
        perror("Errore in alimentatore per chiudere la memoria condivisa ");
        exit(EXIT_FAILURE);
    }
    printf("CHECKPOINT: Scollegamento dalla memoria!\n");

}
/**
 * E' Presente un problema: 
 *      Siccome l'array è stato inizializzato dal master, all'interno della struct mettiamo soltanto il puntatore
 *      ma il vettore effettivo è all'interno dell'area privata del processo master e quindi l'alimentatore avendo il 
 *      puntatore all'array non riesce ad accedere.
*/

void add_int_to_shared_array(struct memCond* shared_struct, int pid) {
   // key_t key = ftok("Modules", 1);
    // Calcola la dimensione del nuovo vettore
    //int new_size = (shared_struct->nAtomi + 1) * sizeof(int);

    // Crea un nuovo segmento di memoria condivisa per il vettore

    
    printf("NUMERO ATOMI: %d\n",shared_struct->nAtomi);
    printf("Numero di bytes: %d\n",shared_struct->nAtomi * sizeof(int));
    /*int old_shm_id = shmget(KEY_ARRAY_CONDIVISO,(sizeof(int) * shared_struct->nAtomi), IPC_CREAT | 0666);

     

    if (old_shm_id == -1) {
        //printf("Errore nella creazione della nuova memoria condivisa ");
        perror("Error: \n");
        exit(EXIT_FAILURE);
    }*/
    int old_shm_id = shared_struct->id_vettore_condiviso;





    //Recupera array condiviso:

    int* old_array = (int*) shmat(old_shm_id, NULL, 0);
    if(old_array == NULL){
        perror("Error: %d");
    }

    int* private_array = malloc(sizeof(int) * shared_struct->nAtomi);

    memcpy(private_array,old_array,sizeof(int)*(shared_struct->nAtomi));
    /*for(int i = 0; i<shared_struct->nAtomi;i++){
        private_array[i] = old_array[i]; 
    }*/
    //printf("Puntatore del vecchio array: %p\n",old_array);
    //printf("Puntatore dell' array privato: %p\n",private_array);
    // Elimina l'array condiviso
    if (shmdt(old_array) == -1) {
        printf("shmdt\n");
        exit(EXIT_FAILURE);
    }
    if(shmctl (old_shm_id , IPC_RMID , NULL ) == -1){
        printf("Errore di deallocazione nell'alimentatore del vettore condiviso \n");
        exit(EXIT_FAILURE);
    }
     
    shared_struct->nAtomi = shared_struct->nAtomi + 1;

    // Creazione dell'array condiviso di dimensione n + 1

    int new_shm_id = shmget(KEY_ARRAY_CONDIVISO, shared_struct->nAtomi, IPC_CREAT | 0666);

    if(new_shm_id == -1){
        perror("Errore della creazione dell'array di interi  ");
    }
    //Aggiornamento id nuovo array con dimensione ( n + 1 )

    shared_struct->id_vettore_condiviso = new_shm_id;
    

    //int* temp_array = malloc(sizeof(int)*(shared_struct->nAtomi));

    int * new_array = (int*) shmat(new_shm_id, NULL, 0);
    //printf("Primo processo: %d",new_array[0]);
    if (new_array == NULL) {

        //printf("Errore nel collegamento della nuova memoria condivisa \n");
        
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
    memcpy(new_array, private_array, (shared_struct->nAtomi-1) * sizeof(int));
    // Aggiungi il nuovo intero al vettore

    new_array[shared_struct->nAtomi-1] = pid;

    //printf("Pid del primo processo: %d\n",new_array[0]);

    // Scollega e rilascia il vecchio segmento di memoria condivisa per il vettore

    //shmdt(new_array);

    // Aggiorna la struct in memoria condivisa per usare il nuovo vettore

    //shared_struct->vPid = new_array;
    
    /*
    shmdt(new_array);
    shmctl ( new_shm_id , 0 , NULL ) ;
    */

    if(shmdt ( new_array ) == -1){
        perror("Errore nella chiusura dell'array condiviso nell'alimentatore: ");
        exit(EXIT_FAILURE);
    }
    
    
}