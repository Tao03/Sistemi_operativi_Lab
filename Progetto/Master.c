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
    printf("Master avviato\n");
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
        
    }
    
    printf("Attivatore avviato\nAlimentatore avviato\n");
    struct sembuf my_op;
    int idSemaforo = setSemaforo();
    my_op.sem_num = 0; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */

    /**
     * Inizializzazione della memoria condivisa
     *
     */
    int idMemoriaCondivisa = setMemoriaCondivisa(N_ATOMI_INIT);


    /**
     * Creazione processi atomi iniziali
     */
    creaAtomi(N_ATOMI_INIT, N_ATOMO_MAX, idMemoriaCondivisa);
    semop(idSemaforo, &my_op, 1);

    /**
     * CHANGELOG: il master non è più in sola lettura, ma deve aggiornare
     * la memoria condivisa con l'energia prelevata, aggiunto controllo semaforo prioritario
     */

    my_op.sem_num = 1; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */
    semop(idSemaforo, &my_op, 1);

    my_op.sem_num = 2; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */
    semop(idSemaforo, &my_op, 1);

    /**
     *Accesso alla memoria condivisa solo in lettura per la stampa, il master può accedervi in lettura
     *solo per stampare le statistiche, quindi non è necessario controllare i semafori.
     * CHANGELOG: non vale più, il master deve accedere alla memoria condivisa per aggiornare l'energia consumata
     */
    int check = 0;
    pidAllarme = fork();
    if (pidAllarme == 0)
    {
        char *const array[2] = {"10", 0};
        execv("Alarm", array);
        perror("");
        exit(1);
    }
    printf("Allarme avviato\n");
    while (check == 0 && tempoScaduto == 0 && forkError == 0)
    {

        alarm(TIMER_PRELEVA);
        wait(NULL);

        if (tempoScaduto == 0 && forkError ==  0)
        {
            if (flag == 0)
            {
                pause();
            }
            my_op.sem_num = 1; /* only one semaphore in array of semaphores */
            my_op.sem_flg = 0; /* no flag : default behavior */
            my_op.sem_op = -1; /* accessing the resource */
            semop(idSemaforo, &my_op, 1);

            
            prelevaEnergia(ENERGY_CONSUMPTION);
            stampa();

            struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
            
            check = checkEnergia();

            my_op.sem_op = 1; /* releasing the resource */
            semop(idSemaforo, &my_op, 1);
            flag = 0;
        }
    }
    kill(pidAlimentatore, SIGKILL);
    kill(pidAttivatore, SIGHUP);
    kill(pidInibitore, SIGKILL);
    my_op.sem_num = 1; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1; /* accessing the resource */
    semop(idSemaforo, &my_op, 1);
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    int idArrayCondiviso = datap->id_vettore_condiviso;
    int *array = shmat(idArrayCondiviso, NULL, 0);
    for (int i = 0; i < datap->nAtomi; i++)
    {
        kill(array[i], SIGKILL);
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
