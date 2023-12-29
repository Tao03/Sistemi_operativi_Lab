#include "../Headers/master.h"
#include "../Headers/risorse.h"

int setSemaforo()
{
    int id = semget(KEY_SEMAFORO, 3, 0); // Prova a ottenere l'ID del semaforo esistente

    if (id != -1) // Se l'ID non è -1, il semaforo esiste
    {
        // Rimuovi il semaforo esistente
        printf("Semaforo esistente trovato, rimozione...\n");
        if (semctl(id, 0, IPC_RMID) == -1)
        {
            perror("Errore nella rimozione del semaforo: ");
            exit(EXIT_FAILURE);
        }
    }

    // Crea un nuovo semaforo
    id = semget(KEY_SEMAFORO, 3, IPC_CREAT | IPC_EXCL | 0666);
    if (id == -1)
    {
        perror("Errore nella creazione del semaforo: ");
        exit(EXIT_FAILURE);
    }

    // Imposta i valori iniziali dei semafori
    semctl(id, 0, SETVAL, -1);/* Semaforo sincronizazzione-1 impostato a 1 solo per test*/
    semctl(id, 1, SETVAL, 1); // Semaforo prioritario
    semctl(id, 2, SETVAL, 1); // Semaforo per atomi
    //printf("Valore semaforo sincronizzazione: %d\n",semctl(id, 0, GETVAL, 0)); funziona

    return id;
}

int setMemoriaCondivisa(int nKids) // id = 32819
{
    //printf("Dimensione del dummy: %ld\n",sizeof(dummy));
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    /**
     * Si crea un array di interi condiviso
    */
    if(idMemoriaCondivisa == -1){
        perror("Errore nella creazione della memoria condivisa: ");
        exit(EXIT_FAILURE);
    }
    struct memCond * datap ; /* shared data struct */
    datap = shmat(idMemoriaCondivisa, NULL, 0);

   /**
    * Una cosa da notare è che quando l'alimentatore aumenta la dimensione dell'array condiviso, riavviando il master porta 
    * ad un errore perchè il master prenderà la memoria condivisa ma con una dimensione vecchia e non quella recente siccome non è aggiornata
   */
    datap->nAtomi = nKids;
    datap->scorie = 0;
    datap->eTot = ENERGY_DEMAND;
    datap->nScissioni = 0;
    datap->nAttivazioni = 0;
    datap->eConsumata = 0;
    int id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO, 0, 0);
    //printf("Id array condiviso: %d\n",id_array_condiviso);

    id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO, sizeof(int)*datap->nAtomi, IPC_CREAT | 0666);
   
    if (id_array_condiviso == -1)
    {
        perror("Errore nella creazione dell'array condiviso: ");
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
        perror("Errore nello scollegamento della memoria condivisa: ");
    }

    if(statusArray == -1){
        perror("Errore nello scollegamento dell'array condivisa: \n");
    }

    printf("CHECKPOINT: MEMORIA CONDIVISA CREATA CON SUCCESSO!\n");
    return idMemoriaCondivisa;
}

void creaAtomi(int nAtomi, int nAtomoMax, int idMemoriaCondivisa)
{
    for (int i = 0; i < nAtomi; i++)
    {



        
        int pid = fork();
        if (pid == 0)
        {



            //printf("CHECKPOINT: processo atomo creato con pid: %d\n",getpid());
            srand(getpid());
            int numeroAtomico = rand()%nAtomoMax;
            char stringa[100];
            sprintf(stringa, "%d", numeroAtomico);
            char  * const array[2] = {stringa,0};
            execv("Atomo",array);
            perror("");
            exit(EXIT_SUCCESS);


            
        }else{
            /**
             * Porzione di codice che consiste nel aggiungere il pid degli atomi in memoria condivisa
            */
           //printf("Passo il pid [%d]\n",pid);
           insertAtomi(i,pid,idMemoriaCondivisa);
        }
        
    }
    //printf("CHECKPOINT: la creazione degli atomi è andata a buon fine!\n");
    /**
     * Fino a qui, il codice funziona correttamente
    */

    
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
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    //datap->nAtomi = datap->nAtomi + 1;

    
    /**
     * QUI SI DEVE INSERIRE LA PARTE DI CODICE CHE INSERISCE I PROCESSI
    */
    int * arrayPid = shmat(datap->id_vettore_condiviso,NULL,0);
    if(arrayPid == NULL){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }

    arrayPid[indice] = pid;

    //printf("Aggiunto %d\n",arrayPid[indice]);
    int status = shmdt (arrayPid);
     
    if(status == -1){
        perror("Error: ");
    }
    status = shmdt(datap);
    if(status == -1){
        perror("Erro: ");
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
        fprintf( stderr," Errore nel collegamento della memoria condivisa\n");
    }
    struct memCond * datap = shmat(idMemoriaCondivisa,NULL,0);
    if(datap == NULL){
        fprintf(stderr," Errore, la memoria condivisa è null \n");
    }
    datap->eTot  = datap->eTot - eneryDemand;
    
    shmdt(datap);
    //printf("Energia prelevata! \n");
}

int checkEnergia()
{
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);
    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);
    if (datap == NULL)
    {

        fprintf(stderr, "Processo master in stampa shmget memoria condivisa");

        exit(EXIT_FAILURE);
    }
    int value = 0;
    if (datap->eTot > ENERGY_EXPLODE_THRESHOLD)
    {
        value = 1;
    }
    if (datap->eTot < 0)
    {
        value = 2;
    }
    if (shmdt(datap) == -1)
    {
        fprintf(stderr, "Processo master in checkEnergia shmdt");
        exit(EXIT_FAILURE);
    }
    return value;
}

void stampa()
{
    printf("----------------------STATISTICHE----------------------\n");
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(dummy), IPC_CREAT | 0666);

    struct memCond *datap = shmat(idMemoriaCondivisa, NULL, 0);

    if (datap == NULL)
    {

        fprintf(stderr, "Processo master in stampa shmget memoria condivisa");

        exit(EXIT_FAILURE);
    }

    printf("Numero di atomi: %d\n", datap->nAtomi);
    printf("Scorie totali: %d\n", datap->scorie);
    printf("Energia totale: %d\n", datap->eTot);
    printf("Numero di scissioni: %d\n", datap->nScissioni);
    printf("Numero di attivazioni: %d\n", datap->nAttivazioni);
    printf("Energia consumata: %d\n", datap->eConsumata);
    

    /*int idArrayCondiviso = shmget(KEY_ARRAY_CONDIVISO, sizeof(int)*datap->nAtomi, IPC_CREAT | 0666);




    if (idArrayCondiviso == -1) {
        fprintf(stderr,"Processo master in stampa shmget array condiviso");
        printf("Numero atomi %d\n",datap->nAtomi);
        exit(EXIT_FAILURE);
    }*/

    // int id_array_condiviso = shmget(KEY_ARRAY_CONDIVISO,10,IPC_CREAT | 0666);
    int *array = shmat(datap->id_vettore_condiviso, NULL, 0);

    if (array == (int *)-1)
    {
        fprintf(stderr, "Processo master in stampa shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < datap->nAtomi; i++)
    {

        printf("Pid del processo atomo: [%d]\n", array[i]);
    }
    printf("ENERGIA TOTALE: %d\n", datap->eTot);
    printf("-------------------------------------------------------\n");

    // Non dimenticare di staccare l'area di memoria condivisa quando hai finito

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
}