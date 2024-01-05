#include "../Headers/master.h"
#include "../Headers/risorse.h"
struct sembuf my_op;
int idSemaforo;

/*Operazione sul semaforo per decrementare*/
void P(int nSem){
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        fprintf(stderr,"Errore sul semaforo: ");
    }
}
/*Operazione sul semaforo per incrementare*/
void V(int nSem){
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1;  /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        fprintf(stderr,"Errore sul semaforo: ");
    }
}

int setSemaforo()
{
    // Prova a ottenere l'ID del semaforo esistente
    idSemaforo = semget(KEY_SEMAFORO, 3, 0); 

    if (idSemaforo != -1) // Se l'ID non è -1, il semaforo esiste
    {
        // Rimuovi il semaforo esistente
        if (semctl(idSemaforo, 0, IPC_RMID) == -1)
        {
            fprintf(stderr,"Errore nella rimozione dell'array di semafori\n, linea %d\n",__LINE__);
            exit(EXIT_FAILURE);
        }
    }

    // Crea un nuovo semaforo
    idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | IPC_EXCL | 0666);
    if (idSemaforo == -1)
    {
        fprintf(stderr,"Errore nella creazione dell'array di semafori\n, linea %d\n",__LINE__);
        exit(EXIT_FAILURE);
    }

    // Imposta i valori iniziali dei semafori
    semctl(idSemaforo, 0, SETVAL, -1);/* Semaforo per la sincronizazzione */
    semctl(idSemaforo, 1, SETVAL, 1); // Semaforo prioritario
    semctl(idSemaforo, 2, SETVAL, 1); // Semaforo per atomi

    return idSemaforo;
}

/**
 * Il metodo consiste nel creare 2 segmenti di memoria condivisi a tutti i processi
 * Il primo serve a tenere traccia delle informazioni relative alle statisiche come numero scissioni, energia totale ecc.
 * Invece il secondo ci serve per tenere traccia dei Processe Identifier dei processi atomo
*/
int setMemoriaCondivisa() // id = 32819
{
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    /**
     * Si crea un array di interi condiviso
    */
    if(idMemoriaCondivisa == -1){
        fprintf(stderr,"Errore nella creazione della memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    struct memCond * datap ; /* shared data struct */
    datap = shmat(idMemoriaCondivisa, NULL, 0);
    /**
     * Inizializzazione dei valori della memoria condivisa
    */
    datap->nAtomi = N_ATOMI_INIT;
    datap->scorie = 0;
    datap->eTot = ENERGY_DEMAND;
    datap->nScissioni = 0;
    datap->nAttivazioni = 0;
    datap->eConsumata = 0;
    datap->pidInibitore = 0;
    datap->scorieUtilmoSecondo = 0;
    datap->nAttivazioniUltimoSecondo = 0;
    datap->nScissioniUltimoSecondo = 0;
    datap->eTotUltimoSecondo = 0;
    int id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO, 0, 0);

    id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO, sizeof(int)*datap->nAtomi, IPC_CREAT | 0666);
   
    if (id_array_condiviso == -1)
    {
        fprintf(stderr,"Errore nella creazione delL'array condiviso, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    /**
     * Appena creata la memoria condivisa, nessun processo è in fase di scrittura quindi si potrebbe tranquillamente
     * entrare all'interno della memoria condivisa senza utilizzare semafori
    */
   
    int* new_array = (int*) shmat(id_array_condiviso, NULL, 0);

    datap->id_vettore_condiviso = id_array_condiviso;

    int status = shmdt (datap);

    int statusArray = shmdt (new_array);

    if(status == -1){
        fprintf(stderr,"Errore nello scollegamento della memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }

    if(statusArray == -1){
        fprintf(stderr,"Errore nello scollegamento dell'array condiviso, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    return idMemoriaCondivisa;
}

void creaAtomi(int nAtomi,  int idMemoriaCondivisa)
{
    for (int i = 0; i < N_ATOMI_INIT; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            srand(getpid());
            int numeroAtomico = rand()%N_ATOMICO_MAX;
            char stringa[100];
            sprintf(stringa, "%d", numeroAtomico);
            char ppid[100];
            sprintf(ppid, "%d", getppid());
            char  * const array[3] = {stringa,ppid,0};
            execv("Atomo",array);
            perror("");
            exit(EXIT_SUCCESS);


            
        }else if(pid == -1){
            kill(getpid(),SIGUSR2);
            fprintf(stderr,"Errore nel creare un processo attraverso la fork, linea %d\n",__LINE__);
            exit(EXIT_FAILURE);
        }
        else{
             insertAtomi(i,pid,idMemoriaCondivisa);
        }
    }

    
}
/**
 * Dato i 3 parametri, il metodo consiste nel registrare il Process identifier del processo atomo nell'array condiviso:
 * 
 * 1)indice: indica la posizione da inserire all'interno del vettore
 * 2)pid: il valore effettivo che deve essere inserito
 * 3)idMemoriaCondivisa: l'identificatore della memoria condivisa
*/
void insertAtomi(int indice, int pid, int idMemoriaCondivisa){
    struct sembuf my_op ;
    my_op . sem_num = 0; 
    my_op . sem_flg = 0; 
    my_op . sem_op = -1; 
    semop ( 1234 , & my_op , 1) ; 
    struct memCond * datap ; 
    // Si ottiene la memoria condivisa
    datap = shmat ( idMemoriaCondivisa, NULL ,0) ;
    if (datap == (struct memCond *)(-1)) {
        fprintf(stderr,"Errore nel ottenere dla memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    // Si ottiene il vettore condiviso
    int * arrayPid = shmat(datap->id_vettore_condiviso,NULL,0);
    if(arrayPid == NULL){
        fprintf(stderr,"Errore nel ottenere la memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    // Inserimento effettivo
    arrayPid[indice] = pid;

    
    int status = shmdt (arrayPid);
     
    if(status == -1){
        fprintf(stderr,"Errore nel scollegarsi dal vettore condiviso, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    status = shmdt(datap);
    if(status == -1){
        fprintf(stderr,"Errore nel scollegarsi dalla memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    // Si esce dalla sezione critica
   my_op . sem_op = 1; 
   semop ( 1234 , & my_op , 1);
}

/**
 * Il metodo consiste nel prelevare una quantita definita dal parametro energyDemand di energia 
*/
void prelevaEnergia(int energyDemand){
    //Si ottiene l'identificatore della memoria condivisa
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA,sizeof(dummy),IPC_CREAT|0666);
    if(idMemoriaCondivisa == -1){
        fprintf( stderr," Errore nel ottenere l'identificatore della memoria condivisa, linea %d \n",__LINE__);
        exit(EXIT_FAILURE);
    }
    //Si ottiene la memoria condivisa
    struct memCond * datap = shmat(idMemoriaCondivisa,NULL,0);
    if(datap == NULL){
        fprintf(stderr,"Errore processo Master: collegamento memoria condivisa in linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    //Si fa un prelievo effettivo dell'energia
    datap->eTot  = datap->eTot - energyDemand;
    datap->eConsumata = datap->eConsumata + energyDemand;
    
    shmdt(datap);
}

/**
 * Il metodo consiste nel controllare se la simulazione è andata in EXPLODE oppure in BLACKOUT
*/
int checkEnergia()
{
    //Si ottiene l'identificatore della memoria condivisa
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    if(idMemoriaCondivisa == -1){
        fprintf(stderr,"Errore nel ottenere la memoria condivisa, linea %d \n",__LINE__);
        exit(EXIT_FAILURE);
    }
    //Si ottiene la memoria condivisa effettiva
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    if (datap == NULL)
    {

        fprintf(stderr, "Errore processo Master: collegamento memoria condivisa in linea %d",__LINE__);

        exit(EXIT_FAILURE);
    }
    int value = 0;
    /**
     * Nel caso l'energia totale fosse superiore alla soglia massima, il flag value viene asserito a 1
    */
    if (datap->eTot > ENERGY_EXPLODE_THRESHOLD)
    {
        value = 1;
    }
    /**
     * Nel caso l'energia totale è negativa, il flag value viene asserito a 2
    */
    if (datap->eTot < 0)
    {
        value = 2;
    }
    //Scollegamento dalla memoria condivisa
    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Errore processo Master: scollegamento memoria condivisa in linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    return value;
}
/**
 * Metodo che consiste nel inserire il pid dell'inibitore nella memoria condivisa
*/
void inserisciInibitore(int pidInibitore)
{
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    if (datap == NULL)
    {

        fprintf(stderr, "Processo master in stampa shmget memoria condivisa in linea %d",__LINE__);

        exit(EXIT_FAILURE);
    }
    datap->pidInibitore = pidInibitore;
    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Errore processo Master: scollegamento memoria condivisa in linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }

}
/**
 * Il metodo consiste nel visualizzare le statistiche richieste nella consegna
*/
void stampa()
{
    /*system("clear");*/

    printf("----------------------STATISTICHE----------------------\n");
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);

    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);

    if (datap == NULL)
    {

        fprintf(stderr, "Processo master in stampa shmget memoria condivisa in linea %d",__LINE__);

        exit(EXIT_FAILURE);
    }

    printf("Numero di atomi: %d\n", datap->nAtomi);
    printf("Scorie totali: %d, nell'ultimo secondo c'è stato un incremento di %d scorie\n", datap->scorie,datap->scorieUtilmoSecondo);
    printf("Energia totale: %d, nell'ultimo secondo: %d\n", datap->eTot, datap->eTotUltimoSecondo);
    printf("Numero di scissioni: %d, nell'ultimo secondo ci sono stati %d scissioni in più\n", datap->nScissioni,datap->nScissioniUltimoSecondo);
    printf("Numero di attivazioni: %d, nell'ultimo secondo ci sono state %d attivazioni in più\n", datap->nAttivazioni,datap->nAttivazioniUltimoSecondo);
    printf("Energia consumata: %d\n", datap->eConsumata);
    datap->scorieUtilmoSecondo = 0;
    datap->eTotUltimoSecondo = 0;
    datap->nScissioniUltimoSecondo = 0;
    datap->nAttivazioniUltimoSecondo = 0;
    int *array = shmat(datap->id_vettore_condiviso, NULL, 0);

    if (array == (int *)-1)
    {
        fprintf(stderr, "Errore processo Master: collegamento della memoria condivisa in linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    printf("ENERGIA TOTALE: %d\n", datap->eTot);
    printf("-------------------------------------------------------\n");

    /**
     * Scollegamento dell'array condiviso 
    */
    if (shmdt(array) == -1)
    {
        fprintf(stderr, "Errore processo Master: scollegamento dell'array  condiviso in linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    /**
     * Scollegamento della memoria condivisa 
    */
    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Errore processo Master: scollegamento della memoria condivisa in linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
}
void terminazione(int idSemaforo,int idMemoriaCondivisa,int check,int tempoScaduto,int forkError, int pidAlimentatore, int pidAttivatore,int pidInibitore, char* argv[],int argc){
    struct sembuf my_op;
    kill(pidAlimentatore, SIGKILL);
    waitpid(pidAlimentatore,NULL,0);
    kill(pidAttivatore, SIGKILL);
    waitpid(pidAttivatore,NULL,0);
    if (argc > 1 && strcmp(argv[1], "inibitore") == 0)
    {
        kill(pidInibitore, SIGKILL);
    }
    V(1);
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    int idArrayCondiviso = datap->id_vettore_condiviso;
    int *array = shmat(idArrayCondiviso, NULL, 0);
    for (int i = 0; i < datap->nAtomi; i++)
    {
       if(array[i]!=-1){
        kill(array[i],SIGKILL);
       }
        
    }


    P(1);

    if (shmdt(array) == -1)
    {
        fprintf(stderr, "Errore nello scollegamento del vettore condiviso, linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }

    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Errore nello scollegamento del segmento condiviso, linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    if (shmctl(idMemoriaCondivisa, IPC_RMID, NULL) == -1)
    {
        fprintf(stderr, "Errore nel deallocare il segmento condiviso, linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    if (shmctl(idArrayCondiviso, IPC_RMID, NULL) == -1)
    {
        fprintf(stderr, "Errore nello deallocare il vettore condiviso, linea %d",__LINE__);
        exit(EXIT_FAILURE);
    }
    if (semctl(idSemaforo, 0, IPC_RMID) == -1)
    {
        fprintf(stderr, "Errore nello deallocare il vettore di semafori, linea %d",__LINE__);
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
    printf("La simulazione è andata a buon fine senza errori!\n");
}
