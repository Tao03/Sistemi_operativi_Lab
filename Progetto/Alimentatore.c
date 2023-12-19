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
#define TIMER 2
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
    printf("Alarm\n");
    while(1) //va sostituito con l'attesa di terminazione dal master
    {


        alarm(TIMER);
        pause();
        printf("ciao\n");
        creaAtomi(nKids);
        flag = 0;

        
    }
    
}
void handle_signal(int signum){
    //printf("CHECKPOINT: segnale arrivato\n");
    //printf("CHECKPOINT: HANDLE FINITO\n");
    flag = 1;
}