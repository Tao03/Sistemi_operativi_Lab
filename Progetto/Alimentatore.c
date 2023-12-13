# define _GNU_SOURCE /* necessary from now on */
# define TIMER 2
# include "Headers/alimentatore.h"
# include <signal.h>
# include <string.h>
# include <stdio.h>

//int sigaction ( int signum , const struct sigaction * act , struct sigaction * oldact );
int nKids;
void creaAtomi(int nkids);
int flag=0;


int main(int argc, char* argv[])
{
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
        while(flag==0){}
        creaAtomi(nKids);
        flag = 0;
    }
    
}
void handle_signal(int signum){
   /// printf("CHECKPOINT: segnale arrivato\n");
    //printf("CHECKPOINT: HANDLE FINITO\n");
    flag = 1;
}