#define _GNU_SOURCE
#include "../Headers/master.h"
#include "../Headers/risorse.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}
int check(int *array, int size)
{
    int index = -1;
    for (int i = 0; i < size && index == -1; i++)
    {
        if (*(array + i) == -1)
        {
            index = i;
        }
    }
    return index;
}
int find(int elem,int * array,int n)
{
    int index = -1;
    for(int i = 0;i<n && index == -1;i++){
        if(array[i]==elem){
            index = i;
        }
    }
    return index;
}
void aggiungiAtomo(int pid, int energiaLiberata)
{
    struct memCond *shared_struct; /* shared data struct */
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(shared_struct), IPC_CREAT | 0666);
    if (idMemoriaCondivisa == -1)
    {
        perror("Error in shmeg ");
        exit(EXIT_FAILURE);
    }
    shared_struct = (struct memCond *)shmat(idMemoriaCondivisa, NULL, 0);
    if (shared_struct == NULL)
    {
        perror("Errore in shmat ");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    int old_shm_id = shared_struct->id_vettore_condiviso;
    int *old_array = (int *)shmat(old_shm_id, NULL, 0);
    if (old_array == NULL)
    {
        perror("Error: %d");
    }

    int index = check(old_array, shared_struct->nAtomi);
    if (index != -1)
    {
        old_array[index] = pid;
        if (shmdt(old_array) == -1)
        {
            printf("shmdt\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int *private_array = malloc(sizeof(int) * shared_struct->nAtomi);
        memcpy(private_array, old_array, sizeof(int) * (shared_struct->nAtomi));
        /*for(int i = 0; i<shared_struct->nAtomi;i++){
            private_array[i] = old_array[i];
        }*/
        // printf("Puntatore del vecchio array: %p\n",old_array);
        // printf("Puntatore dell' array privato: %p\n",private_array);
        //  Elimina l'array condiviso
        if (shmdt(old_array) == -1)
        {
            printf("shmdt\n");
            exit(EXIT_FAILURE);
        }
        if (shmctl(old_shm_id, IPC_RMID, NULL) == -1)
        {
            fprintf(stderr,"Errore di deallocazione nell'alimentatore del vettore condiviso \n");
            exit(EXIT_FAILURE);
        }

        shared_struct->nAtomi = shared_struct->nAtomi + 1;
        shared_struct->eTot = shared_struct->eTot + energiaLiberata;
        // Creazione dell'array condiviso di dimensione n + 1

        int new_shm_id = shmget(KEY_ARRAY_CONDIVISO, shared_struct->nAtomi, IPC_CREAT | 0666);

        if (new_shm_id == -1)
        {
            perror("Errore della creazione dell'array di interi  ");
        }
        // Aggiornamento id nuovo array con dimensione ( n + 1 )

        shared_struct->id_vettore_condiviso = new_shm_id;

        // int* temp_array = malloc(sizeof(int)*(shared_struct->nAtomi));

        int *new_array = (int *)shmat(new_shm_id, NULL, 0);
        // printf("Primo processo: %d",new_array[0]);
        if (new_array == NULL)
        {

            // printf("Errore nel collegamento della nuova memoria condivisa \n");

            exit(EXIT_FAILURE);
        }
        // Copia i dati dal vecchio vettore al nuovo
        memcpy(new_array, private_array, (shared_struct->nAtomi - 1) * sizeof(int));
        // Aggiungi il nuovo intero al vettore

        new_array[shared_struct->nAtomi - 1] = pid;

        if (shmdt(new_array) == -1)
        {
            perror("Errore nella chiusura dell'array condiviso nell'alimentatore: ");
            exit(EXIT_FAILURE);
        }
    }
}

void scissione(int* nAtomico, int argc, char *argv[])
{
    /**
     * Se il numero atomico dell'atomo nuovo è un valore casuale tra [1,N_ATOMICO_MAX] 
     * Può essere che il numero atomodico del processo padre sia negativo, cosa si può fare?
     * Per ora facciamo che non possa essere negativo però 
     * il numero atomico del figlio + numero atomico del padre = numero atomico del padre prima della scissione
     *  e per farlo basta assegnare al processo atomo nuovo un valore casuale tra [1,numero atomico del padre prima della scissione]
    */
    int nAtomicoFiglio = rand() % *nAtomico; // numero atomico del figlio
    *nAtomico -= nAtomicoFiglio;                  // aggiorno il numero atomico del padre
    printf("ATOMO %d: Numero atomico del padre: %d\n",getpid(),*nAtomico);
    int id = semget(KEY_SEMAFORO, 1, 0666); // ottengo id del semaforo
    struct sembuf my_op;
    int pid = fork();
    if (pid == 0) // è il figlio
    {
        sprintf(argv[0], "%d", nAtomicoFiglio); // converto il numero atomico in stringa (per passarlo come parametro)
        // argv[1]=nAtomicoFiglio;
        execve("Atomo", argv, NULL); //<- esegue il figlio con il nuovo numero atomico
        perror("Errore: ");
        exit(1);
    }
    else // è il padre
    {
        my_op.sem_num = 1; // scelgo il semaforo prioritario
        my_op.sem_flg = 0;
        my_op.sem_op = -1;    // occupo il semaforo
        semop(id, &my_op, 1); // eseguo le operazioni

        my_op.sem_num = 2; // scelgo il semaforo atomi
        my_op.sem_flg = 0;
        my_op.sem_op = -1;    // occupo il semaforo
        semop(id, &my_op, 1); // eseguo le operazioni
        // sezione critica inizio

        // calcolo l'energia liberata dalla scissione
        int energiaLiberata = nAtomicoFiglio * (*nAtomico) - max(nAtomicoFiglio, *nAtomico);

        // aggiung il pid del figlio nel vettore dei pid e aggiorno l'energia
        aggiungiAtomo(pid, energiaLiberata);

        // sezione critica fine

        my_op.sem_op = 1;     // rilascio il semaforo atomo
        semop(id, &my_op, 1); // eseguo le operazioni

        my_op.sem_num = 1;    // scelgo il semaforo prioritario
        my_op.sem_op = 1;     // rilascio il semaforo prioritario
        semop(id, &my_op, 1); // eseguo le operazioni
    }
}
void removePid(int pid,int idSemaforo, int nAtomico){
    struct sembuf my_op;

    my_op.sem_num = 2; // scelgo il semaforo di sincronizzazione
    my_op.sem_flg = 0;
    my_op.sem_op = -1; // occupo il semaforo
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }
    my_op.sem_num = 1; // scelgo il semaforo di sincronizzazione
    my_op.sem_op = -1; // occupo il semaforo
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }

    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA,sizeof(dummy),IPC_CREAT | 0666);
    if(idMemoriaCondivisa == -1){
        fprintf(stderr, "Errore nel ottenere l'identificatore della memoria condivisa");
        exit(EXIT_FAILURE);
    }
    struct memCond * datap = shmat(idMemoriaCondivisa,NULL,0);
    datap->scorie = datap->scorie + nAtomico;
    int* array = shmat(datap->id_vettore_condiviso,NULL,0);
    int index = find(pid,array,datap->nAtomi);
    if(index != -1){
        array[index] = -1;
        printf("ATOMO %d: Sono appena stato eliminato\n",getpid());
    }else{
        printf("Errore, sembra che il processo da uccidere non esista\n");
    }

    my_op.sem_num =1; // scelgo il semaforo di sincronizzazione
    my_op.sem_flg = 0;
    my_op.sem_op = 1; // occupo il semaforo
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }
    my_op.sem_num = 2; // scelgo il semaforo di sincronizzazione
    my_op.sem_op = 1; // occupo il semaforo
    if (semop(idSemaforo, &my_op, 1) == -1)
    {
        fprintf(stderr, "Errore nell'accesso col semaforo di partenza\n");
        exit(EXIT_FAILURE);
    }

   
}



