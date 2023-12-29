#include "Headers/attivatore.h"
#include "Headers/risorse.h"


void handle_signal(int signal)
{
    //flag=1;   
}
void handle_sighup(int signal)
{
    //master=0;
    //printf("Sono attivatore e ho ricevuto SIGHUP\n");
    kill(getpid(),SIGKILL);
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
    //while (semctl(sem_id, 0, GETVAL)==-1){}
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
    //printf("Programma sincronizzato\n");
    alarm(TIMER_ATTIVATORE);
    while(1) //finchè il processo master non termina
    {
        //printf("PID attivatore: %d\n",getpid());
        alarm(TIMER_ATTIVATORE);
        pause();
        printf("SONO ATTIVATORE NEL CICLO\n");
        scegliAtomoVittima();  
        printf("ATOMO SCELTO\n");
        //controllo che il semaforo prioritario sia libero
        //accedo a memoria condivisa
    }
    //printf("SONO ATTIVATORE E TERMINO\n");
}