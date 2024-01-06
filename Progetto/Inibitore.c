#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include "Headers/risorse.h"

#define MSG_TYPE 1
struct sembuf my_op;
int idSemaforo;
// Variabile globale per la coda di messaggi
int coda;
/*Operazione sul semaforo per decrementare*/
void P(int nSem){
    idSemaforo = semget(KEY_SEMAFORO, 3, IPC_CREAT | 0666);
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = 1;  /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        fprintf(stderr,"Errore sul semaforo: %d, linea %d\n",errno,__LINE__);
    }
}
/*Operazione sul semaforo per incrementare*/
void V(int nSem){
    my_op.sem_num = nSem; /* only one semaphore in array of semaphores */
    my_op.sem_flg = 0; /* no flag : default behavior */
    my_op.sem_op = -1;  /* accessing the resource */
    if(semop ( idSemaforo , & my_op , 1) == -1){
        fprintf(stderr,"Errore sul semaforo: %d, linea %d\n",errno,__LINE__);
    }
}
void prelevaEnergia(int energia){
    V(1);
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
    datap->energiaAssorbita = datap->energiaAssorbita + energia;
    
    shmdt(datap);
    P(1);

}
// Funzione per gestire il segnale SIGUSR2
void handleSIGUSR2(int sig) {
    // Genera un numero casuale tra 0 e 1
    srand(time(NULL));
    int esito = rand() % 2;

    // Crea un messaggio con l'energia da sottrarre e l'esito
    struct messaggio msg;
    msg.tipo = MSG_TYPE;
    msg.energia = 10; // Esempio: sottrae 1 di energia
    msg.esito = esito;
    
    // Invia il messaggio alla coda di messaggi del processo che ha inviato il segnale
    msgsnd(coda, &msg, sizeof(struct messaggio) - sizeof(long), 0);
    prelevaEnergia(msg.energia);
    if(esito == 0){
        printf("OPERAZIONE INIBITORE: Energia prelevata: %d, la scissione ha avuto esito positivo \n",msg.energia);
    }else{
        printf("OPERAZIONE INIBITORE: Energia prelevata: %d, la scissione ha avuto esito negativo \n",msg.energia);
    }
}

// Funzione per gestire il segnale di pausa
void handlePause(int sig) {
    // Mette in pausa l'esecuzione
    pause();
}


int main() {
    // Crea la coda di messaggi
    coda = msgget(KEY_CODA_MESSAGGI, IPC_CREAT | 0666);

    // Registra la gestione del segnale SIGUSR2
    struct sigaction sa;

    sa.sa_handler = handleSIGUSR2;

    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        fprintf(stderr,"errore inibitore SIGUSR2\n");
        exit(1);
    }

    // Registra la gestione del segnale di pausa

    sa.sa_handler = handlePause;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        fprintf(stderr,"errore inibitore SIGTSTP\n");
        exit(1);
    }
    P(0);
    V(0);
    
    // Esecuzione del programma
    while (1) {
        pause();
    }

    // Rimuovi la coda di messaggi alla fine dell'esecuzione
    msgctl(coda, IPC_RMID, NULL);

    return 0;
}
