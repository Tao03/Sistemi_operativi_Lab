#include "Headers/master.h"
#include "Headers/risorse.h"
void terminazione(int idSemaforo,int idMemoriaCondivisa,int check, char* argv[],int argc);
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
        perror("Errore ");
        exit(1);
    }
    pidAlimentatore = fork();
    if (pidAlimentatore == 0)
    {
        
        char *const array[3] = {"2", 0};
        execv("Alimentatore", array);
        perror("");
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
            printf("Inibitore avviato\n");
            char *const array[2] = {"0", 0};
            execv("Inibitore", array);
            perror("");
            exit(1);
        }
        else if(pidInibitore == -1)
        {
            perror("Errore nella fork di inibitore");
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
    my_op.sem_num = 0; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */
    semop(idSemaforo, &my_op, 1);


    
    int check = 0;
    /**
     * Creazione del processo Alarm che si occupa soltanto di inviare il segnale al processo master che indica il timeout
    */
    pidAllarme = fork();
    if (pidAllarme == 0)
    {
        char *const array[1] = {0};
        execv("Alarm", array);
        perror("");
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
            my_op.sem_num = 1; /* only one semaphore in array of semaphores */
            my_op.sem_flg = 0; /* no flag : default behavior */
            my_op.sem_op = -1; /* accessing the resource */
            semop(idSemaforo, &my_op, 1);

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
            my_op.sem_op = 1; /* releasing the resource */
            semop(idSemaforo, &my_op, 1);
            /**
             *Il flag viene rimesso a 0 altrimenti riesegue la porzione di codice senza aspettare il segnale
            */
            flag = 0;
        }
        
    }

terminazione(idSemaforo,idMemoriaCondivisa,check,argv,argc);


    
    
}
void terminazione(int idSemaforo,int idMemoriaCondivisa,int check, char* argv[],int argc){
    struct sembuf my_op;
    kill(pidAlimentatore, SIGKILL);
    waitpid(pidAlimentatore,NULL,0);
    printf("Il processo  alimentatore ha terminato correttamente\n");
    kill(pidAttivatore, SIGKILL);
    waitpid(pidAttivatore,NULL,0);
    printf("Il processo attivatore ha terminato correttamente\n");
    if (argc > 1 && strcmp(argv[1], "inibitore") == 0)
    {
        kill(pidInibitore, SIGKILL);
    }
    my_op.sem_num = 1; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1; /* accessing the resource */
    semop(idSemaforo, &my_op, 1);
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    int idArrayCondiviso = datap->id_vettore_condiviso;
    int *array = shmat(idArrayCondiviso, NULL, 0);
    for (int i = 0; i < datap->nAtomi; i++)
    {
       if(array[i]!=-1){
        kill(array[i],SIGKILL);
        //wait(NULL);
       }
        
        printf("Processo atomo con pid %d terminato correttamente\n",array[i]);
    }

    my_op.sem_num = 1; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* releasing the resource */
    semop(idSemaforo, &my_op, 1);

    if (shmdt(array) == -1)
    {
        fprintf(stderr, "Processo master in stampa shmdt");
        exit(EXIT_FAILURE);
    }

    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Processo master in stampa shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(idMemoriaCondivisa, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    if (shmctl(idArrayCondiviso, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    if (semctl(idSemaforo, 0, IPC_RMID) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    if (check == 1)
    {
        printf("La simulazione è terminata per il seguente motivo: l'energia totale ha superato il limite massimo\n");
    }
    if (check == 2)
    {
        printf("La simulazione è terminata per il seguente motivo: l'energia totale non è sufficiente per i prossimi prelievi di energia\n");
    }
    if (tempoScaduto == 1)
    {
        printf("La simulazione è terminata per il seguente motivo: il tempo dedicato alla simulazione è finito\n");
    }
    if(forkError == 1){
        printf("La simulazione è terminata per il seguente motivo: errore nelle fork dei processi\n");
    }
}
