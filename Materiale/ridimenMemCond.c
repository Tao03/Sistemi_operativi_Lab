#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#define KEY 1234

struct memCond {
    int* array;
    int num_elements;
    int another_int;
};

void add_int_to_shared_array(struct memCond* shared_struct, int new_int) {
    // Calcola la dimensione del nuovo vettore
    int new_size = (shared_struct->num_elements + 1) * sizeof(int);

    // Crea un nuovo segmento di memoria condivisa per il vettore
    int new_shm_id = shmget(KEY, new_size, IPC_CREAT | 0666);
    if (new_shm_id == -1) {
        perror("Errore nella creazione della nuova memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // Collega il nuovo segmento di memoria condivisa al tuo spazio di indirizzi
    int* new_array = (int*) shmat(new_shm_id, NULL, 0);
    if (new_array == (void*) -1) {
        perror("Errore nel collegamento della nuova memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // Copia i dati dal vecchio vettore al nuovo
    memcpy(new_array, shared_struct->array, shared_struct->num_elements * sizeof(int));

    // Aggiungi il nuovo intero al vettore
    new_array[shared_struct->num_elements] = new_int;

    // Scollega e rilascia il vecchio segmento di memoria condivisa per il vettore
    shmdt(shared_struct->array);
    shmctl(shm_id, IPC_RMID, NULL);

    // Aggiorna la struct in memoria condivisa per usare il nuovo vettore
    shared_struct->array = new_array;
    shared_struct->num_elements++;
}

int main() {
    // Qui dovresti allocare la memoria condivisa per la struct e il vettore come nell'esempio precedente

    // Poi, quando vuoi aggiungere un intero al vettore, chiama la funzione add_int_to_shared_array
    add_int_to_shared_array(shared_struct, new_int);

    return 0;
}