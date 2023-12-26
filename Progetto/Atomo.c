#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "Headers/atomo.h"
#include "Headers/risorse.h"

void handler(int signal)
{
    printf("Sono entrato nell'handler\n");
}
int main(int argc, char* argv[]){
    printf("Sono il processo atomo e sono stato eseguito!\n");
    int nAtomico = strtol(argv[0],NULL,10); //numero atomico
    printf("Il mio numero atomico è: %d \n",nAtomico);

    //inizializzo l'handler
    struct sigaction sa;
    sigset_t my_mask;
    
    //bzero(&sa, sizeof(sa));
    sa.sa_handler=&handler;
    if(sigaction(SIGUSR1, &sa, NULL) == -1)//SIGUSR1 è il segnale che viene inviato all'atomo per farlo scindere
    {
        fprintf(stderr, "Errore inizializzazione handler Atomo\n");
    }
    while (1) //da sostituire col segnale di terminazione
    {
        //l'atomo non fa nulla se non viene effettuata la scissione
        /*while(flag==0)
        */
        
        pause();
        printf("Sono l'atomo e sono riuscito a ricevere il segnale\n");
        if(nAtomico>N_ATOMICO_MIN)
        {
            printf("Sono l'atomo e ho abbastanza energia per scindermi\n");
            scissione(nAtomico, argc, argv);
        }
        else
        {
            printf("Sono l'atomo e non ho abbastanza energia per scindermi\n");
            exit(EXIT_SUCCESS);
            /*ATOMO CHE MUORE*/
        }
    }
    
    
    

}
