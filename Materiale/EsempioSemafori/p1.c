#include <sys/sem.h>
#include <stdio.h>

int main() {
    key_t key = ftok("/tmp", 'a');
    int semid = semget(key, 1, 0666 | IPC_CREAT);

    struct sembuf my_op;
    my_op.sem_num = 0;
    my_op.sem_flg = 0;
    my_op.sem_op = -1; // Acquire the semaphore

    semop(semid, &my_op, 1);

    printf("Program 1 is printing\n");

    my_op.sem_op = 1; // Release the semaphore
    semop(semid, &my_op, 1);

    return 0;
}
/**
 * In entrambi i programmi, prima di eseguire l'operazione di stampa, acquisiscono il semaforo impostando `sem_op` 
 * a -1 e chiamando `semop`. Dopo aver eseguito l'operazione di stampa, rilasciano il semaforo impostando `sem_op` a 
 * 1 e chiamando `semop`. Questo garantisce che solo un programma alla volta possa eseguire l'operazione di stampa.
 * 
 * Se il semaforo è già occupato (il suo valore è 0) e un processo tenta di acquisirlo 
 * (impostando `sem_op` a -1 e chiamando `semop`), il processo sarà bloccato fino a quando il semaforo non sarà rilasciato 
 * da un altro processo.Nel tuo codice, alla riga 71, stai tentando di acquisire il semaforo. Se il semaforo è già occupato, 
 * il tuo processo sarà bloccato a questa riga fino a quando il semaforo non sarà rilasciato. Una volta che il semaforo 
 * è rilasciato, il tuo processo acquisirà il semaforo, decrementando il suo valore a 0, e procederà con l'esecuzione 
 * del codice successivo.
*/