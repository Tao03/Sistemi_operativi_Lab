#include <stdio.h>
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
#include <signal.h>
#include "Headers/risorse.h"
void handle_signal(int signal){
    
}
int main(int argc, char * argv[]){

    struct sigaction s1;
    void handle_signal(int signal);
    memset(&s1, 0, sizeof(s1));    /* set all bytes to zero */
    s1.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &s1, NULL); /* CASE 1: set new handler */


    printf("Timer avviato per %d secondi.\n", SIM_DURATION);


    alarm(SIM_DURATION);
    pause();
    kill(getppid(),SIGUSR1);

    return 0;
}