#include "Headers/master.h"
#include <sys/shm.h>
#include <stdio.h>
#include "Headers/risorse.h"
void main()
{
    



    /**
     * Inizializzazione dei 3 semafori
    */
    struct sembuf my_op ;
    int idSemaforo = setSemaforo();
    my_op . sem_num = 2; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
   printf("Ho appena dato il via!\n");
   semop ( idSemaforo , & my_op , 1) ; 
    /**
     * Inizializzazione della memoria condivisa
    */
    int idMemoriaCondivisa=setMemoriaCondivisa(N_ATOMI_INIT);
    printf("id della memoria condivisa: %d\n",idMemoriaCondivisa);
    int pid = fork();
    if(pid == 0){
        char  * const array[2] = {"2",0};
        execv("Alimentatore",array);
        perror("");
        exit(1);
    }
    printf("PID dell'alimentatore: %d\n",pid);
    /**
     * Creazione processi atomi
    */
   creaAtomi(N_ATOMI_INIT,N_ATOMO_MAX,idMemoriaCondivisa);



  
    my_op . sem_num = 2; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = 1; /* accessing the resource */

   printf("Ho appena dato il via!\n");
   semop ( idSemaforo , & my_op , 1) ; 
   
   while(1){
    sleep(1);
     my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    semop ( idSemaforo , & my_op , 1) ; 
    

    stampa();
    
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = 1; /* accessing the resource */
    semop ( idSemaforo , & my_op , 1) ; 



   }

   
}




void stampa(){

    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA,sizeof(dummy),IPC_CREAT | 0666);

    struct memCond * datap= shmat(idMemoriaCondivisa,NULL,0);
    
    if(datap == NULL){

        perror("Processo master in stampa shmget memoria condivisa ");
        
        exit(EXIT_FAILURE);

    }

    printf("Numero di atomi: %d\n",datap->nAtomi);



    /*int idArrayCondiviso = shmget(KEY_ARRAY_CONDIVISO, sizeof(int)*datap->nAtomi, IPC_CREAT | 0666);




    if (idArrayCondiviso == -1) {
        perror("Processo master in stampa shmget array condiviso");
        printf("Numero atomi %d\n",datap->nAtomi);
        exit(EXIT_FAILURE);
    }*/

   // int id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO,10,IPC_CREAT | 0666);
    int *array = shmat(datap->id_vettore_condiviso, NULL, 0);

    if (array == (int *)-1) {
        perror("Processo master in stampa shmat");
        exit(EXIT_FAILURE);
    }

    
    for (int i = 0; i < datap->nAtomi; i++) {

        printf("Pid del processo atomo: [%d]\n", array[i]);

    }
    printf("-------------------------------------------------\n\n\n");

    // Non dimenticare di staccare l'area di memoria condivisa quando hai finito
    if (shmdt(array) == -1) {
        perror("Processo master in stampa shmdt");
        exit(EXIT_FAILURE);
    }



    if (shmdt(datap) == -1) {
        perror("Processo master in stampa shmdt");
        exit(EXIT_FAILURE);
    }




}