#define _GNU_SOURCE
#include "Headers/attivatore.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>


#define clock 60
void handle_signal(int signal)
{
    //controllo che il semaforo prioritario sia libero
    
        
    //accedo a memoria condivisa
    
    scegliAtomoVittima();
}

int main(int argc, char * argv[])
{
    struct sigaction sa;
    sigset_t my_mask;
    //bzero(&sa, sizeof(sa));
    sa.sa_handler=&handle_signal;

    if(sigaction(SIGALRM, &sa, NULL) == -1)
    {
        fprintf(stderr, "Failed setting handler\n");
    }
    while(1) //va sostituito con l'attesa di terminazione dal master
    {
        alarm(clock);
    }
}