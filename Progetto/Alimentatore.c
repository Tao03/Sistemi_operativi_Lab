# define _GNU_SOURCE /* necessary from now on */
# define TIMER 2
# include "Headers/alimentatore.h"
# include <signal.h>
# include <string.h>
# include <stdio.h>

//int sigaction ( int signum , const struct sigaction * act , struct sigaction * oldact );
int nKids;
void handle_signal(int signum){
    printf("CHECKPOINT: segnale arrivato");
    for(int i = 0;i<nKids;i++){
        int pid = fork();
        printf("CHECKPOINT: processo con pid %d è stato appena generato",getpid());
        if(pid==0){
            /**
             * In questa parte del codice ci sarà l'aggiunta dei processi atomo in memoria condivisa
            */
          // execv("atomo.c","ciao");
        }
    }
}

int main(int argc, char* argv[])
{
    nKids = strtol(argv[0],NULL,10);

    void handle_signal(int signal); /* the handler */
    struct sigaction new, old;     
    memset(&new,0,sizeof(new));     /* set all bytes to zero */
    new.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &new, NULL);  /* CASE 1: set new handler */
    printf("Alarm\n");
    alarm(TIMER);
}