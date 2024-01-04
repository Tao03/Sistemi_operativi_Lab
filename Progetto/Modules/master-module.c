#include "../Headers/master.h"
#include "../Headers/risorse.h"

int setSemaforo()
{
    int id = semget(KEY_SEMAFORO, 3, 0); // Prova a ottenere l'ID del semaforo esistente

    if (id != -1) // Se l'ID non è -1, il semaforo esiste
    {
        // Rimuovi il semaforo esistente
        if (semctl(id, 0, IPC_RMID) == -1)
        {
            fprintf(stderr,"Errore nella rimozione dell'array di semafori\n, linea %d\n",__LINE__);
            exit(EXIT_FAILURE);
        }
    }

    // Crea un nuovo semaforo
    id = semget(KEY_SEMAFORO, 3, IPC_CREAT | IPC_EXCL | 0666);
    if (id == -1)
    {
        fprintf(stderr,"Errore nella creazione dell'array di semafori\n, linea %d\n",__LINE__);
        exit(EXIT_FAILURE);
    }

    // Imposta i valori iniziali dei semafori
    semctl(id, 0, SETVAL, -1);/* Semaforo sincronizazzione-1 impostato a 1 solo per test*/
    semctl(id, 1, SETVAL, 1); // Semaforo prioritario
    semctl(id, 2, SETVAL, 1); // Semaforo per atomi
    //printf("Valore semaforo sincronizzazione: %d\n",semctl(id, 0, GETVAL, 0)); funziona

    return id;
}

int setMemoriaCondivisa() // id = 32819
{
    //printf("Dimensione del dummy: %ld\n",sizeof(dummy));
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
    * Una cosa da notare è che quando l'alimentatore aumenta la dimensione dell'array condiviso, riavviando il master porta 
    * ad un errore perchè il master prenderà la memoria condivisa ma con una dimensione vecchia e non quella recente siccome non è aggiornata
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
    //printf("Id array condiviso: %d\n",id_array_condiviso);

    id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO, sizeof(int)*datap->nAtomi, IPC_CREAT | 0666);
   
    if (id_array_condiviso == -1)
    {
        fprintf(stderr,"Errore nella creazione delL'array condiviso, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    /**
     * Appena creata la memoria condivisa, nessun processo è in fase di scrittura quindi si potrebbe tranquillamente
     * entrare all'interno della risorsa senza concorrenza
    */
   
    int* new_array = (int*) shmat(id_array_condiviso, NULL, 0);

    

    //datap->vPid = new_array;
    //int new_shm_id = shmget(1222, datap->nAtomi, IPC_CREAT | 0666);
        
    
    //memcpy(new_array, datap->vPid, datap->nAtomi * sizeof(int));
    //int new_shm_id = shmget(1222, datap->nAtomi, IPC_CREAT | 0666);

    datap->id_vettore_condiviso = id_array_condiviso;

    int status = shmdt (datap);

    int statusArray = shmdt (new_array);

    if(status == -1){
        fprintf(stderr,"Errore nello scollegamento della memoria condivisa, linea: %d\n ",__LINE__);
    }

    if(statusArray == -1){
        fprintf(stderr,"Errore nello scollegamento dell'array condiviso, linea: %d\n ",__LINE__);
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



            //printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
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
            exit(EXIT_SUCCESS);
        }
        else{
             insertAtomi(i,pid,idMemoriaCondivisa);
        }
    }

    
}
void insertAtomi(int indice, int pid, int idMemoriaCondivisa){
    struct sembuf my_op ;
    my_op . sem_num = 0; /* only one semaphore in array of semaphores */
    my_op . sem_flg = 0; /* no flag : default behavior */
    my_op . sem_op = -1; /* accessing the resource */
    semop ( 1234 , & my_op , 1) ; /* blocking if others hold resource */
    //printf("CHECKPOINT: Accesso alla memoria condivisa!\n");
    //printf("ID dell'array di semafori: %d, valore semaforo %d\n",id, semctl(id, 0, GETVAL));
    struct memCond * datap ; /* shared data struct */
    datap = shmat ( idMemoriaCondivisa, NULL ,0) ;
    if (datap == (struct memCond *)(-1)) {
        fprintf(stderr,"Errore nel ottenere dla memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }
    //datap->nAtomi = datap->nAtomi + 1;

    
    /**
     * QUI SI DEVE INSERIRE LA PARTE DI CODICE CHE INSERISCE I PROCESSI
    */
    int * arrayPid = shmat(datap->id_vettore_condiviso,NULL,0);
    if(arrayPid == NULL){
        fprintf(stderr,"Errore nel ottenere la memoria condivisa, linea: %d\n ",__LINE__);
        exit(EXIT_FAILURE);
    }

    arrayPid[indice] = pid;

    //printf("Aggiunto %d\n",arrayPid[indice]);
    int status = shmdt (arrayPid);
     
    if(status == -1){
        fprintf(stderr,"Errore nel scollegarsi dal vettore condiviso, linea: %d\n ",__LINE__);
    }
    status = shmdt(datap);
    if(status == -1){
        fprintf(stderr,"Errore nel scollegarsi dalla memoria condivisa, linea: %d\n ",__LINE__);
    }
    //printf("Numero atomi: %d\n",datap->nAtomi);
    /**
     * 
     * 
     * 
     * SEZIONE CRITICA
     * 
     * 
     * 
     * 
    */
   my_op . sem_op = 1; /* releasing the resource */
   semop ( 1234 , & my_op , 1) ; /* may un - block others */
   
   //printf("CHECKPOINT: Rilascio memoria condivisa\n\n");
}

void prelevaEnergia(int eneryDemand){

    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA,sizeof(dummy),IPC_CREAT|0666);
    if(idMemoriaCondivisa == -1){
        fprintf( stderr," Errore nel ottenere l'identificatore della memoria condivisa, linea %d \n",__LINE__);
    }
    struct memCond * datap = shmat(idMemoriaCondivisa,NULL,0);
    if(datap == NULL){
        fprintf(stderr,"Errore processo Master: collegamento memoria condivisa in linea %d",__LINE__);
    }
   
    datap->eTot  = datap->eTot - eneryDemand;
    datap->eConsumata = datap->eConsumata + eneryDemand;
    
    shmdt(datap);
}

int checkEnergia()
{
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    if(idMemoriaCondivisa == -1){
        fprintf(stderr,"Errore nel ottenere la memoria condivisa, linea %d \n",__LINE__);
    }
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    if (datap == NULL)
    {

        fprintf(stderr, "Errore processo Master: collegamento memoria condivisa in linea %d",__LINE__);

        exit(EXIT_FAILURE);
    }
    int value = 0;
    /**
     * Nel caso l'energia totale fosse superiore alla soglia massima, il flag value viene asserito
    */
    if (datap->eTot > ENERGY_EXPLODE_THRESHOLD)
    {
        value = 1;
    }
    /**
     * Nel caso l'energia totale è negativa, il flag value viene asserito per uscire
    */
    if (datap->eTot < 0)
    {
        value = 2;
    }
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

void stampa()
{
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

    // Non dimenticare di staccare l'area di memoria condivisa quando hai finito
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