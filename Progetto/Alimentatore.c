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
# include "Headers/alimentatore.h"
# include <signal.h>
# include <string.h>

// Viene asserito quando viene avviato l'allarme
int flagAlarm=0;

// Viene asserito quando la simulazione termina
int exitSignal = 0;

void handle_signal(int signum){
    flagAlarm = 1;
}
void handle_exit(int signum){
    exitSignal = 1;
}
/**
 * Il main si occupa di impostare gli handler per gestire i segnali
 * Periodicamente lancia un segnale di tipo alarm a se stesso per asserire a 1 il flag che indica che l'alimentatore 
 * può creare N_NUOVI_ATOMI processi atomo
 * Il segnale SIGUSR1 è stato impostato per avvisare l'alimentatore da parte del master che la simulazione è terminata
 * Per non far terminare l'alimentatore quando sta creando atomi, si è messo un controllo dopo il metodo.
 *  
*/
int main(int argc, char* argv[])
{

   
    struct sigaction new, s_exit;     
    memset(&new,0,sizeof(new));     /* set all bytes to zero */
    new.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &new, NULL);  /* CASE 1: set new handler */
    
    memset(&s_exit,0,sizeof(s_exit));     /* set all bytes to zero */
    s_exit.sa_handler = handle_exit; /* set the handler */
    sigaction(SIGUSR1, &s_exit, NULL);  /* CASE 1: set new handler */

    int idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
    

    //Il processo alimentatore appena vede che il valore del semaforo è strettamente maggiore di 0 può partire la sua esecuzione
    V(0);

    //Usando soltanto V(0)  si bloccherebbe tutti gli altri processi che aspettano il via, quindi lo incrementiamo
    P(0);


    while(exitSignal == 0) //va sostituito con l'attesa di terminazione dal master
    {

        
        alarm(STEP_ALIMENTAZIONE);
        if(exitSignal == 1){
            exit(EXIT_SUCCESS);
        }
        //Si aspetta i processi figli atomo creati dall'alimentatore
        wait(NULL);

        /**Nel caso si riceve la terminazione di un qualsiasi processo figlio atomo prima di ricevere il segnale alarm
         * Si mette in pause fino a quando non si riceve il segnale
         * **/
        if(flagAlarm == 0){
            pause();
        }
        if(exitSignal == 1){
            exit(EXIT_SUCCESS);
        }
        creaAtomi();
        flagAlarm = 0;
        
        

        
    }
    
}
