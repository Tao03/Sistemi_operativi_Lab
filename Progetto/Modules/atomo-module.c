#define _GNU_SOURCE
#include "../Headers/master.h"
#include "../Headers/risorse.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>

struct sembuf my_op;
int idSemaforo;

/*Operazione sul semaforo per decrementare*/
void P(int nSem){
    idSemaforo = semget(KEY_SEMAFORO, 1, 0666); // ottengo id del semaforo
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        fprintf(stderr,"Errore sul semaforo: ");
    }
}
/*Operazione sul semaforo per incrementare*/
void V(int nSem){
    idSemaforo = semget(KEY_SEMAFORO, 1, 0666); // ottengo id del semaforo
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1;  /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        fprintf(stderr,"Errore sul semaforo: ");
    }
}

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
        fprintf(stderr,"Error in shmeg ");
        exit(EXIT_FAILURE);
    }
    shared_struct = (struct memCond *)shmat(idMemoriaCondivisa, NULL, 0);
    if (shared_struct == NULL)
    {
        fprintf(stderr,"Errore in shmat ");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    int old_shm_id = shared_struct->id_vettore_condiviso;
    int *old_array = (int *)shmat(old_shm_id, NULL, 0);
    if (old_array == NULL)
    {
        fprintf(stderr,"Errore nel collegarsi al vettore condiviso, errore %d, linea %d",errno,__LINE__);
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
        shared_struct->nScissioni = shared_struct->nScissioni + 1;
        shared_struct->nScissioniUltimoSecondo = shared_struct->nScissioniUltimoSecondo + 1;
    }
    else
    {
        int *private_array = malloc(sizeof(int) * shared_struct->nAtomi);
        memcpy(private_array, old_array, sizeof(int) * (shared_struct->nAtomi));
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
            fprintf(stderr,"Errore della creazione dell'array di interi  ");
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
            fprintf(stderr,"Errore nella chiusura dell'array condiviso nell'alimentatore: ");
            exit(EXIT_FAILURE);
        }
        //da qui sono sicuro che la scissione sia avvenuta con successo
        shared_struct->nScissioni = shared_struct->nScissioni + 1;
        shared_struct->nScissioniUltimoSecondo = shared_struct->nScissioniUltimoSecondo + 1;
    }
}

void scissione(int* nAtomico, int argc, char *argv[])
{
    srand(time(NULL));
    int nAtomicoFiglio = rand() % *nAtomico; // numero atomico del figlio
    *nAtomico -= nAtomicoFiglio;                  // aggiorno il numero atomico del padrE
    int id = semget(KEY_SEMAFORO, 1, 0666); // ottengo id del semaforo
    struct sembuf my_op;

    
    V(2); //-1
    V(1);
        
    // sezione critica inizio
    struct memCond *shared_struct;
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA, sizeof(shared_struct), IPC_CREAT | 0666);
    if (idMemoriaCondivisa == -1)
    {
        fprintf(stderr,"Error in shmeg ");
        exit(EXIT_FAILURE);
    }
    shared_struct = (struct memCond *)shmat(idMemoriaCondivisa, NULL, 0);
    if (shared_struct == NULL)
    {
        fprintf(stderr,"Errore in shmat ");
        exit(EXIT_FAILURE);
    }
    int esito = 1;
    int energia = 0;
    if(shared_struct->pidInibitore != 0)
    {
        kill(shared_struct->pidInibitore, SIGUSR2);

        //si collega alla coda di messaggi

        struct messaggio msg;
        int coda = msgget(KEY_CODA_MESSAGGI, IPC_CREAT | 0666);
        // Aspetta un messaggio sulla coda di messaggi
        msgrcv(coda, &msg, sizeof(msg) - sizeof(long), 0, 0);
        esito = msg.esito;
        energia = msg.energia;
    }
    

    if(esito == 1)
    {
        int pid = fork();
        if (pid == 0) 
        {
            sprintf(argv[0], "%d", nAtomicoFiglio); // converto il numero atomico in stringa (per passarlo come parametro)
            execve("Atomo", argv, NULL); //<- esegue il figlio con il nuovo numero atomico
            fprintf(stderr,"Errore: ");
            exit(1);
        }
        // Nel caso la fork fallisce si esce per meltdown
        else if(pid == -1){
             fprintf(stderr,"Errore nella fork dell'atomo, linea %d ",__LINE__);
             kill(SIGUSR2,strtol(argv[1],NULL,10));
        }else{
            // calcolo l'energia liberata dalla scissione
            int energiaLiberata = nAtomicoFiglio * (*nAtomico) - max(nAtomicoFiglio, *nAtomico) - energia;

            // aggiung il pid del figlio nel vettore dei pid e aggiorno l'energia
            aggiungiAtomo(pid, energiaLiberata);
        }
    }
    

    // sezione critica fine
    P(1);
    P(2);
}
void removePid(int pid, int nAtomico){
    V(2);
    V(1);
    
    int idMemoriaCondivisa = shmget(KEY_MEMORIA_CONDIVISA,sizeof(dummy),IPC_CREAT | 0666);
    if(idMemoriaCondivisa == -1){
        fprintf(stderr, "Errore nel ottenere l'identificatore della memoria condivisa");
        exit(EXIT_FAILURE);
    }
    struct memCond * datap = shmat(idMemoriaCondivisa,NULL,0);
    datap->scorie = datap->scorie + nAtomico;
    datap->scorieUtilmoSecondo = datap->scorieUtilmoSecondo + nAtomico;
    int* array = shmat(datap->id_vettore_condiviso,NULL,0);
    int index = find(pid,array,datap->nAtomi);
    if(index != -1){
        array[index] = -1;
    }else{
        printf("Errore, sembra che il processo da uccidere non esista\n");
    }
    P(1);
    P(2);

   
}



