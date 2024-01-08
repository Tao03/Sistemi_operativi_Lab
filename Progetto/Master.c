#include "Headers/master.h"
#include "Headers/risorse.h"
int flag = 0;
int tempoScaduto = 0;
int forkError = 0;
int pidAlimentatore;
int pidAttivatore;
int pidInibitore;
int pidAllarme;
void handle_signal(int signal)
{
    flag = 1;
}
void handle_exit(int signal)
{
    tempoScaduto = 1;
}
void handle_fork_error(int signal)
{
    forkError = 1;
}

void main(int argc,char * argv[])
{
    /**
     * Inizializzazione dei 3 hanlder per i seguenti segnali:
     * 1) il primo serve per visualizzare ogni secondo le informazioni principali relative alla simulazione
     * 2) il secondo viene inviato da un processo chiamato allarme e il segnale indica la terminazione per timeout
     * 3) il terzo serve per segnale un errore durante una qualsiasi fork
    */
    void handle_signal(int signal); /* the handler */
    void handle_exit(int signal);
    void handle_fork_error(int signal);

    struct sigaction s1, s2,s3;
    memset(&s1, 0, sizeof(s1));    /* set all bytes to zero */
    s1.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &s1, NULL); /* CASE 1: set new handler */

    memset(&s2, 0, sizeof(s2));    /* set all bytes to zero */
    s2.sa_handler = handle_exit;   /* set the handler */
    sigaction(SIGUSR1, &s2, NULL); /* CASE 1: set new handler */

    memset(&s3, 0, sizeof(s3));    /* set all bytes to zero */
    s3.sa_handler = handle_fork_error;   /* set the handler */
    sigaction(SIGUSR2, &s3, NULL); /* CASE 1: set new handler */

    /**
     * Creazione dei processi seguenti
     * Attivatore,Alimentatore
    */
    pidAttivatore = fork();
    if (pidAttivatore == 0)
    {
        char *const dummy[2] = {"0", 0};
        execv("Attivatore", dummy);
        fprintf(stderr, "Errore nel  creare il processo attivatore, linea %d",__LINE__);
        exit(1);
    }
    pidAlimentatore = fork();
    if (pidAlimentatore == 0)
    {
        
        char *const array[1] = { 0};
        execv("Alimentatore", array);
        fprintf(stderr, "Errore nel creare il processo alimentatore, linea %d",__LINE__);
        exit(1);
    }

    
    
    /**
     * il metodo setSemaforo serve a creare ( oppure ad ottenere ) il vettore di semafori e inizializzarlo 
    */
    int idSemaforo = setSemaforo();
   

    /**
     * Inizializzazione della memoria condivisa
     *
     */
    int idMemoriaCondivisa = setMemoriaCondivisa();

    /**
     * Creazione del processo inibitore
    */
    if (argc > 1 && strcmp(argv[1], "inibitore") == 0)
    {
        
        pidInibitore = fork();
        if(pidInibitore == 0)
        {
            char *const array[2] = {"0", 0};
            execv("Inibitore", array);
            fprintf(stderr, "Errore nel creare il processo inibitore, linea %d",__LINE__);
            exit(1);
        }
        else if(pidInibitore == -1)
        {
            fprintf(stderr,"Errore nella fork di inibitore");
            exit(1);
        }
        else
        {
            inserisciInibitore(pidInibitore);
        }
        
    }
    /**
     * Creazione primi N_ATOMI_INIT processi atomi iniziali
     */
    creaAtomi(N_ATOMI_INIT, idMemoriaCondivisa);
    struct sembuf my_op;

    /**
     * Serve per dare il via a tutti i processi della simulazione
    */
    P(0);


    
    int check = 0;
    /**
     * Creazione del processo Alarm che si occupa soltanto di inviare il segnale al processo master che indica il timeout
    */
    pidAllarme = fork();
    if (pidAllarme == 0)
    {
        char *const array[1] = {0};
        execv("Alarm", array);
        fprintf(stderr, "Errore nel creare il processo alarm, linea %d",__LINE__);
        exit(1);
    }



    /**
     * CORPO DEL CODICE
    */
    while (check == 0 && tempoScaduto == 0 && forkError == 0)
    {
        /**
         * Periodicamente viene inviato un segnale per prelevare energia
        */
        alarm(TIMER_PRELEVA);
        /**
         * Aspetta che i processi figli atomo terminino altrimenti diventano zombie
        */
        wait(NULL);

        /**
         * Nel caso le 2 condizioni di uscita vengono soddisfatte, non ha senso proseguire e si procede a terminare la simulazione
        */
        if (tempoScaduto == 0 && forkError ==  0)
        {
            /**
             * la variabile flag viene utilizzata per determinare se il segnale è stato ricevuto
             * Nel caso ci fosse stato il segnale allora si procede a prelevare energia 
             * altrimenti si aspetta con pause
            */
            if (flag == 0)
            {
                pause();
            }
            /**
             * Prima di entrare in sezione critica
            */
            V(1);

            //SEZIONE CRITICA: INIZIO

            prelevaEnergia(ENERGY_CONSUMPTION);
            stampa();
            struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
            /**
             * Controlla se l'energia totale è inferiore a 0 oppure ha superato la soglia massima 
            */
            check = checkEnergia();

            //SEZIONE CRITICA: FINE

            /*
            *Rilascio del semaforo
            */
            P(1);
            /**
             *Il flag viene rimesso a 0 altrimenti riesegue la porzione di codice senza aspettare il segnale
            */
            flag = 0;
        }
        
    }
terminazione(idSemaforo,idMemoriaCondivisa,check,tempoScaduto,forkError,pidAlimentatore,  pidAttivatore, pidInibitore, argv, argc);


    
    
}
