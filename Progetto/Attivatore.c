#define _GNU_SOURCE
#include "Headers/attivatore.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "Headers/risorse.h"

#define clock 30
int flag=0;
int master=1;
void handle_signal(int signal)
{
    //flag=1;   
}
void handle_sighup(int signal)
{
    master=0;
}

int main()
{
    /**
     * Inizializzazione handler del timer
    */
    struct sigaction sa;
    sigset_t my_mask;
    sa.sa_handler=&handle_signal;
    if(sigaction(SIGALRM, &sa, NULL) == -1)
    {
        fprintf(stderr, "Failed setting handler\n");
    }


    /**
     * Inizializzazione handler di SIGHUP, ricevuto quando il processo master termina
    */
    struct sigaction sa_hup;
    sa_hup.sa_handler = &handle_sighup;
    if (sigaction(SIGHUP, &sa_hup, NULL) == -1) 
    {
        fprintf(stderr, "Failed setting SIGHUP handler\n");
    }

    int sem_id = semget(KEY_SEMAFORO, 0, 0);
    while (semctl(sem_id, 0, GETVAL)==-1){}
    //attende che il master dia il via alla sincronizzazione, da sostituire con segnale di sincronizzazione del master
    printf("Programma sincronizzato\n");
    alarm(clock);
    while(master) //finchè il processo master non termina
    {
        
        pause();
        scegliAtomoVittima();  
        //controllo che il semaforo prioritario sia libero
        //accedo a memoria condivisa
    }
}