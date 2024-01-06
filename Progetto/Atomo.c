#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "Headers/atomo.h"
#include "Headers/risorse.h"
int flag = 0;
void handler(int signal)
{
    flag = 1;
}
int main(int argc, char* argv[]){
    int nAtomico = strtol(argv[0],NULL,10);

    
    V(0);
    P(0);

    //inizializzo l'handler
    struct sigaction sa;
    sa.sa_handler=&handler;
    if(sigaction(SIGUSR2, &sa, NULL) == -1)//SIGUSR1 è il segnale che viene inviato all'atomo per farlo scindere
    {
        fprintf(stderr, "Errore inizializzazione handler Atomo\n");
    }
    while (1) 
    {
        //l'atomo va in pause() se non viene effettuata la scissione
        wait(NULL);
        if(flag == 0){
            pause();
        }
        if(nAtomico>N_ATOMICO_MIN)
        {
            scissione(&nAtomico, argc, argv);
        }
        else
        {
            removePid(getpid(),nAtomico);
            kill(getpid(),SIGKILL);
        }
        flag = 0;
    }
    
    
    

}
