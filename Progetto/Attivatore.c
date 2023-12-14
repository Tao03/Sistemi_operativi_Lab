#define _GNU_SOURCE
#include "Headers/attivatore.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>


#define clock 1
int flag=0;
void handle_signal(int signal)
{
    flag=1;   
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
    alarm(clock);
    while(1) //va sostituito con l'attesa di terminazione dal master
    {
        while (flag==0)
        {}
        //controllo che il semaforo prioritario sia libero
        //accedo a memoria condivisa
        scegliAtomoVittima(); 
        flag=0;       
    }
}