#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <time.h>
#include "Headers/risorse.h"

#define MSG_TYPE 1


// Variabile globale per la coda di messaggi
int coda;

// Funzione per gestire il segnale SIGUSR2
void handleSIGUSR2(int sig) {
    // Genera un numero casuale tra 0 e 1
    srand(time(NULL));
    int esito = rand() % 2;

    // Crea un messaggio con l'energia da sottrarre e l'esito
    struct messaggio msg;
    msg.tipo = MSG_TYPE;
    msg.energia = 1; // Esempio: sottrae 1 di energia
    msg.esito = esito;

    // Invia il messaggio alla coda di messaggi del processo che ha inviato il segnale
    msgsnd(coda, &msg, sizeof(struct messaggio) - sizeof(long), 0);
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


    // Esecuzione del programma
    while (1) {
        // Esempio: esegui altre operazioni qui
    }

    // Rimuovi la coda di messaggi alla fine dell'esecuzione
    msgctl(coda, IPC_RMID, NULL);

    return 0;
}
