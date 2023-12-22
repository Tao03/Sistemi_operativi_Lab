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
void scissione(int nAtomico, int argc, char* argv[]){
    int id=semget(KEY_SEMAFORO, 1, 0666); //ottengo id del semaforo
    struct sembuf my_op ;
    int flag=0;
    do
    {
        my_op . sem_num = 0;//scelgo il semaforo di sincronizzazione
        my_op . sem_flg = 0;
        semop ( id , & my_op , 1) ;//eseguo le operazioni
        if(semctl(id, 0, GETVAL, 0)==1)//se siamo sincronizzati
        {
            
            int nAtomicoFiglio = rand()%N_ATOMICO_MAX;//numero atomico del figlio
            nAtomico-=nAtomicoFiglio;//aggiorno il numero atomico del padre
            
            int pid=fork();
            if(pid == 0) //è il figlio
            {
                sprintf(argv[1], "%d", nAtomicoFiglio);//converto il numero atomico in stringa (per passarlo come parametro)
                //argv[1]=nAtomicoFiglio;
                execve("Atomo",argv,NULL); //<- esegue il figlio con il nuovo numero atomico
            }
            else //è il padre
            {
                int flag2=0;
                do
                {
                    
                        if(semctl(id, 1, GETVAL) && semctl(id, 2, GETVAL))//se il sem. prioritario e quello per gli atomi sono liberi
                        {

                            my_op . sem_num = 1;//scelgo il semaforo prioritario
                            my_op . sem_flg = 0;
                            my_op . sem_op = -1;//occupo il semaforo
                            semop ( id , & my_op , 1) ;//eseguo le operazioni

                            my_op . sem_num = 2;//scelgo il semaforo atomi
                            my_op . sem_flg = 0;
                            my_op . sem_op = -1;//occupo il semaforo
                            semop ( id , & my_op , 1) ;//eseguo le operazioni

                            flag2=1;
                            //sezione critica inizio

                                //calcolo l'energia liberata dalla scissione
                                int energiaLiberata = nAtomicoFiglio*nAtomico-max(nAtomicoFiglio,nAtomico);

                                //aggiung il pid del figlio nel vettore dei pid e aggiorno l'energia
                                //aggiungiAtomo(pid, energiaLiberata);
                                printf("Atomo aggiunto con pid: %d\n",pid);
                            
                            //sezione critica fine

                            my_op . sem_op = 1;//rilascio il semaforo atomo
                            semop ( id , & my_op , 1) ;//eseguo le operazioni
                            
                            my_op . sem_num = 1;//scelgo il semaforo prioritario
                            my_op . sem_op = 1;//rilascio il semaforo prioritario
                            semop ( id , & my_op , 1) ;//eseguo le operazioni
                        }
                } while (flag2==0);  
                
            }
        }
    } while (flag==0);
    
    
    
    

    
    
}