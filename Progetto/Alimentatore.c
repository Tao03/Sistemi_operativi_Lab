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



# include "Headers/alimentatore.h"
# include <signal.h>
# include <string.h>

//int sigaction ( int signum , const struct sigaction * act , struct sigaction * oldact );
int nKids;
int flag=0;
int exitSignal = 0;

int main(int argc, char* argv[])
{

    void handle_signal(int signal); /* the handler */
    void handle_exit(int signal); /* the handler */
    struct sigaction new, s_exit;     
    memset(&new,0,sizeof(new));     /* set all bytes to zero */
    new.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &new, NULL);  /* CASE 1: set new handler */
    
    memset(&s_exit,0,sizeof(s_exit));     /* set all bytes to zero */
    s_exit.sa_handler = handle_exit; /* set the handler */
    sigaction(SIGUSR1, &s_exit, NULL);  /* CASE 1: set new handler */

    struct sembuf my_op ;
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    int idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);



    if(semop ( idSemaforo , & my_op , 1) == -1){
        perror("Errore sul semaforo: ");
    }
    my_op . sem_op = 1; /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        perror("Errore sul   semaforo: ");
    } 


    while(exitSignal == 0) //va sostituito con l'attesa di terminazione dal master
    {

        
        alarm(TIMER_ALIMENTATORE);
        if(exitSignal == 1){
            exit(EXIT_SUCCESS);
        }
        wait(NULL);

        if(flag == 0){
            pause();
        }
        if(exitSignal == 1){
            exit(EXIT_SUCCESS);
        }
        creaAtomi();
        flag = 0;
        
        

        
    }
    
}
void handle_signal(int signum){
    flag = 1;
    //printf("CHECKPOINT: segnale arrivato\n");
    //printf("CHECKPOINT: HANDLE FINITO\n");
}
void handle_exit(int signum){
    exitSignal = 1;
    printf("TERMINAZIONE\n");
    //printf("CHECKPOINT: segnale arrivato\n");
    //printf("CHECKPOINT: HANDLE FINITO\n");
}