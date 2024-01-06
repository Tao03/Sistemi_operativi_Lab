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
    //sigset_t my_mask;
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

    
    //attende che il master dia il via alla sincronizzazione, da sostituire con segnale di sincronizzazione del master
    V(0);
    P(0);

    

    while(1) //finchè il processo master non termina
    {
        
        alarm(STEP_ATTIVATORE);
        pause();
        scegliAtomoVittima();  
        
    }
    
}