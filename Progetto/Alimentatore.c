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

#define TIMER 10

# include "Headers/alimentatore.h"
# include <signal.h>
# include <string.h>

//int sigaction ( int signum , const struct sigaction * act , struct sigaction * oldact );
int nKids;
void creaAtomi(int nkids);
int flag=0;


int main(int argc, char* argv[])
{
    /*
        Per ora mettiamo una costante come numero di processi atomo
        che il processo alimentatore deve generare ma il processo master passerà 
        il numero in argv[0]
    */
    nKids = strtol(argv[0],NULL,10);

    void handle_signal(int signal); /* the handler */
    struct sigaction new, old;     
    memset(&new,0,sizeof(new));     /* set all bytes to zero */
    new.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &new, NULL);  /* CASE 1: set new handler */

    struct sembuf my_op ;
    my_op . sem_num = 2; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    int idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);



    if(semop ( idSemaforo , & my_op , 1) == -1){
        perror("Errore sul semaforo: ");
    } 



    /* blocking if others hold resource */

   // printf("Il master mi ha dato il via!");


    while(1) //va sostituito con l'attesa di terminazione dal master
    {


        alarm(TIMER);
        pause();
        creaAtomi(nKids);

        
    }
    
}
void handle_signal(int signum){
    //printf("CHECKPOINT: segnale arrivato\n");
    //printf("CHECKPOINT: HANDLE FINITO\n");
}