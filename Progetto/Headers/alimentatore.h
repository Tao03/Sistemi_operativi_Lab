//metodo per generare figli(processi atomo)
#include "risorse.h"
/**
 * Metodo che consiste nel generare N_NUOVI_ATOMI processi atomo e inserirli 
 * all'interno del vettore dei pid condiviso attraverso il metodo aggiungiProcessoAtomo()
*/
void creaAtomi();

/**
 * Il metodo aggiungiProcessoAtomo si occupa di aggiungere effettivamente il pid all'interno del vettore condiviso
 * Lo fa attraverso 2 modi:
 *   1) Aumenta la dimensione del vettore condiviso di 1 e occupa la nuova cella con il pid
 *   2) Se all'interno trova una cella che ha come elemento -1, allora modifica quella cella con il pid senza aumentare la dimensione
 * La scelta dei 2 modi dipende tutto dal valore restituito dal metodo check()
 * Il parametro passato è pid ed è il process Identifier del processo da inserire nel vettore
*/
void aggiungiProcessoAtomo(int pid);

/**
 * Il metodo check prende come parametri 2 elementi:
 *   1)array di interi
 *   2)dimensione dell'array
 * Ti restituisce -1 se non esiste un elemento all'interno del vettore con valore -1, nel caso
 * ci fosse un elemento pari a -1, restituisce la posizione dell'elemento all'interno del vettore
 * 
*/
int check(int * array, int size);

/**
 * Il metodo prende come parametri 2 elementi:
 *   1)la struct memCond che ha tutte le informazioni salvate in memoria condivisa
 *   2)un valore interno che indica il Process Identifier di un processo
 * 
 * Consiste nel salvarsi in un array locale il vettore condiviso( con dimensione N ), deallocarlo 
 * e si alloca un altro array con una dimensione N + 1.
 * Successivamente si copiano tutti gli elementi del vettore locale nell'array nuovo e si aggiunge
 * in ultima posizione il valore del parametro pid.
*/
void add_int_to_shared_array(struct memCond* shared_struct, int pid);

/**
 * 
*/
void V(int nSem);

/**
 * 
*/
void P(int nSem);