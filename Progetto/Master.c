#include "Headers/master.h"
#include "Headers/risorse.h"

int flag = 0;
int pidAlimentatore;
int pidAttivatore;
void handle_signal(int signal)
{
    flag = 1;
}
void main()
{
    printf("Master avviato\n");
    void handle_signal(int signal); /* the handler */
    void handle_signal_sigInt(int signal);

    struct sigaction s1, s2;
    memset(&s1, 0, sizeof(s1));    /* set all bytes to zero */
    s1.sa_handler = handle_signal; /* set the handler */
    sigaction(SIGALRM, &s1, NULL); /* CASE 1: set new handler */

    /**
     * Inizializzazione dei 3 semafori
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
        char *const array[2] = {"2", 0};
        execv("Alimentatore", array);
        perror("");
        exit(1);
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
     * Creazione processo alimentatore
     */

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
    while (check == 0)
    {
        
        alarm(TIMER_PRELEVA);
        wait(NULL);
        if (flag == 0)
        {
            pause();
        }
        my_op.sem_num = 1; /* only one semaphore in array of semaphores */
        my_op.sem_flg = 0; /* no flag : default behavior */
        my_op.sem_op = -1; /* accessing the resource */
        semop(idSemaforo, &my_op, 1);
        stampa();
        prelevaEnergia(30);
        struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
        datap->eConsumata = datap->eConsumata + 30;
        check = checkEnergia();

        my_op.sem_op = 1; /* releasing the resource */
        semop(idSemaforo, &my_op, 1);
        flag = 0;
    }
    kill(pidAlimentatore, SIGKILL);
    kill(pidAttivatore, SIGHUP);
    
    my_op.sem_num = 1; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1; /* accessing the resource */
    semop(idSemaforo, &my_op, 1);
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    int idArrayCondiviso = datap->id_vettore_condiviso;
    int *array = shmat(idArrayCondiviso, NULL, 0);
    for (int i = 0; i < datap->nAtomi;i++)
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
    if (shmctl(idMemoriaCondivisa, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    if (shmctl(idArrayCondiviso, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    if (semctl(idSemaforo, 0, IPC_RMID) == -1) {
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
}



