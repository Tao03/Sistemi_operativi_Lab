#include "Headers/attivatore.h"
#include "Headers/risorse.h"


void handle_signal(int signal)
{
    //serve per svegliare il processo
}
void handle_sighup(int signal)
{
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    /**
     * Inizializzazione handler del timer
    */
    struct sigaction sa;
    sigset_t my_mask;
    sa.sa_handler=&handle_signal;
    if(sigaction(SIGALRM, &sa, NULL) == -1)
    {
        fprintf(stderr, "Failed  setting handler\n");
    }


    /**
     * Inizializzazione handler di SIGHUP, ricevuto quando il processo master termina
    */
    struct sigaction sa_hup;
    sa_hup.sa_handler = &handle_sighup;
    if (sigaction(SIGUSR1, &sa_hup, NULL) == -1) 
    {
        fprintf(stderr, "Failed setting SIGHUP handler\n");
    }

    int sem_id = semget(KEY_SEMAFORO, 0, 0);
    struct sembuf my_op ;
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    if(semop(sem_id,&my_op,1)==-1){
        fprintf(stderr,"Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }
    my_op . sem_op = 1; /* accessing the resource */
    if(semop(sem_id,&my_op,1)==-1){
        fprintf(stderr,"Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }
    //attende che il master dia il via alla sincronizzazione, da sostituire con segnale di sincronizzazione del master
    printf("PID ATTIVATORE:%d\n",getpid());
    alarm(TIMER_ATTIVATORE);
    while(1) //finchè il processo master non termina
    {
        
        alarm(TIMER_ATTIVATORE);
        pause();
        
        scegliAtomoVittima();  
        
    }
    
}