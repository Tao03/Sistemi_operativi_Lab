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
    printf("Sono entrato nell'handler\n");
}
int main(int argc, char* argv[]){
    int nAtomico = strtol(argv[0],NULL,10); //numero atomico
    printf("ATOMO %d: Sono un nuovo atomo e il mio numero atomico è: %d\n",getpid(),nAtomico);
    printf("Il pid del processo master è: %ld\n",strtol(argv[1],NULL,10));

    int id = semget(KEY_SEMAFORO, 1, 0666); // ottengo id del semaforo
    struct sembuf my_op;
    my_op.sem_num = 0; // scelgo il semaforo di sincronizzazione
    my_op.sem_flg = 0;
    my_op.sem_op = -1; // occupo il semaforo
    if (semop(id, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }
    my_op.sem_op = 1; // occupo il semaforo
     if (semop(id, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }
    //inizializzo l'handler
    struct sigaction sa;
    sigset_t my_mask;
    
    //bzero(&sa, sizeof(sa));
    sa.sa_handler=&handler;
    if(sigaction(SIGUSR2, &sa, NULL) == -1)//SIGUSR1 è il segnale che viene inviato all'atomo per farlo scindere
    {
        fprintf(stderr, "Errore inizializzazione handler Atomo\n");
    }
    while (1) //da sostituire col segnale di terminazione
    {
        //l'atomo non fa nulla se non viene effettuata la scissione
        /*while(flag==0)
        */


       
        wait(NULL);
        if(flag == 0){
            pause();
        }
        if(nAtomico>N_ATOMICO_MIN)
        {
            printf("ATOMO %d: Ho abbastanza energia per scindermi\n",getpid());
            scissione(&nAtomico, argc, argv);
        }
        else
        {
            printf("ATOMO %d: Non ho abbastanza energia per scindermi quindi mi suicido\n",getpid());
            removePid( getpid(),id,nAtomico);
            kill(getpid(),SIGKILL);
        }
        flag = 0;
    }
    
    
    

}
