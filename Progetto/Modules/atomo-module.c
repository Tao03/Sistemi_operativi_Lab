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
    //printf("Valore semaforo sincronizzazione: %d\n",semctl(id, 0, GETVAL, 0));
    do
    {
        //printf("Sono l'atomo e ho letto il semaforo\n");
        if(semctl(id, 0, GETVAL, 0)==1)//se siamo sincronizzati
        {
            //printf("Atomo sincronizzato\n");
            int nAtomicoFiglio = rand()%N_ATOMICO_MAX;//numero atomico del figlio
            nAtomico-=nAtomicoFiglio;//aggiorno il numero atomico del padre
            
            int pid=fork();
            if(pid == 0) //è il figlio
            {
                char str[10];
                sprintf(str,"%d",nAtomicoFiglio);
                argv[1]=str;
                printf("Sono il figlio ed eseguo atomo");
                execve("Atomo",argv,NULL); //<- esegue il figlio con il nuovo numero atomico
                exit(0);
            }
            else //è il padre
            {
                printf("Sono il padre, controllo i semafori\n");
                int flag2=0;
                do
                {
                    
                        if(semctl(id, 1, GETVAL) && semctl(id, 2, GETVAL))//se il sem. prioritario e quello per gli atomi sono liberi
                        {
                            printf("Sono il padre e ho ottenuto i semafori\n");
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
                                //da togliere il commento->aggiungiAtomo(pid, energiaLiberata);
                                printf("Atomo aggiunto con pid e nAtomico: %d, %d\n",pid, nAtomicoFiglio);
                            
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