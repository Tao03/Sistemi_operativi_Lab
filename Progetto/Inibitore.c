#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <time.h>

#define MSG_TYPE 1

// Struttura del messaggio
struct message {
    long mtype;
    int energy;
    int outcome;
};

// Variabile globale per la coda di messaggi
int msgQueue;

// Funzione per gestire il segnale SIGUSR2
void handleSIGUSR2(int sig) {
    // Genera un numero casuale tra 0 e 2
    srand(time(NULL));
    int outcome = rand() % 3;

    // Crea un messaggio con l'energia da sottrarre e l'esito
    struct message msg;
    msg.mtype = MSG_TYPE;
    msg.energy = 10; // Esempio: sottrae 10 di energia
    msg.outcome = outcome;

    // Invia il messaggio alla coda di messaggi del processo che ha inviato il segnale
    msgsnd(msgQueue, &msg, sizeof(struct message) - sizeof(long), 0);
}

// Funzione per gestire il segnale di pausa
void handlePause(int sig) {
    // Mette in pausa l'esecuzione
    pause();
}

int main() {
    // Crea la coda di messaggi
    msgQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    // Registra la gestione del segnale SIGUSR2
    signal(SIGUSR2, handleSIGUSR2);

    // Registra la gestione del segnale di pausa
    signal(SIGTSTP, handlePause);

    // Esecuzione del programma
    while (1) {
        // Esempio: esegui altre operazioni qui
    }

    // Rimuovi la coda di messaggi alla fine dell'esecuzione
    msgctl(msgQueue, IPC_RMID, NULL);

    return 0;
}
