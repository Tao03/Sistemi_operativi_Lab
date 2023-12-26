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
    if(a>b)
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
    int i=0;
    int old_shm_id = shared_struct->id_vettore_condiviso;
    int *old_array = (int *)shmat(old_shm_id, NULL, 0);
    if (old_array == NULL)
    {
        perror("Error: %d");
    }

    
    int index = check(old_array, shared_struct->nAtomi);
    printf("POSIZIONE: %d\n",index);
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
            printf("Errore di deallocazione nell'alimentatore del vettore condiviso \n");
            exit(EXIT_FAILURE);
        }

        shared_struct->nAtomi = shared_struct->nAtomi + 1;

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

void scissione(int nAtomico, int argc, char* argv[])
{
    int id=semget(KEY_SEMAFORO, 1, 0666); //ottengo id del semaforo
    struct sembuf my_op ;
   
        /*
        my_op . sem_num = 0;//scelgo il semaforo di sincronizzazione
        my_op . sem_flg = 0;
        my_op . sem_op = -1;//occupo il semaforo
        semop ( id , & my_op , 1) ;//eseguo le operazioni
        */
        while (semctl(id, 0, GETVAL, 0) != 1) //attendo che il semaforo sia libero
        {}   
        int nAtomicoFiglio = rand()%N_ATOMICO_MAX;//numero atomico del figlio
        nAtomico-=nAtomicoFiglio;//aggiorno il numero atomico del padre
            
        int pid=fork();
        if(pid == 0) //è il figlio
        {
            sprintf(argv[1], "%d", nAtomicoFiglio);//converto il numero atomico in stringa (per passarlo come parametro)
            //argv[1]=nAtomicoFiglio;
            execve("Atomo",argv,NULL); //<- esegue il figlio con il nuovo numero atomico
            perror("");
            exit(1);
        }
        else //è il padre
        {
            my_op . sem_num = 1;//scelgo il semaforo prioritario
            my_op . sem_flg = 0;
            my_op . sem_op = -1;//occupo il semaforo
            semop ( id , & my_op , 1) ;//eseguo le operazioni

            my_op . sem_num = 2;//scelgo il semaforo atomi
            my_op . sem_flg = 0;
            my_op . sem_op = -1;//occupo il semaforo
            semop ( id , & my_op , 1) ;//eseguo le operazioni
            //sezione critica inizio

                //calcolo l'energia liberata dalla scissione
                int energiaLiberata = nAtomicoFiglio*nAtomico-max(nAtomicoFiglio,nAtomico);

                //aggiung il pid del figlio nel vettore dei pid e aggiorno l'energia
                aggiungiAtomo(pid, energiaLiberata);
                printf("Atomo aggiunto con pid: %d\n",pid);


            //sezione critica fine

            my_op . sem_op = 1;//rilascio il semaforo atomo
            semop ( id , & my_op , 1) ;//eseguo le operazioni
                            
            my_op . sem_num = 1;//scelgo il semaforo prioritario
            my_op . sem_op = 1;//rilascio il semaforo prioritario
            semop ( id , & my_op , 1) ;//eseguo le operazioni
        }
                 
                
}


    
    
    
    
    
