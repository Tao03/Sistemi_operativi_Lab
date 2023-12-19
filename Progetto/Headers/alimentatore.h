//metodo per generare figli(processi atomo)
#include "risorse.h"
void creaAtomi(int nAtomi);
void aggiungiProcessoAtomo(int pid);
void add_int_to_shared_array(struct memCond* shared_struct, int pid);
/**
     * Crea atomi e li aggiunge all'interno della memoria condivisa
     * Metodi da usare:
     *      1) Uso di semafori
     *      2) Accede alla memoria condivisa
     *      3) Inserimento dei atomi in memoria condivisa
*/