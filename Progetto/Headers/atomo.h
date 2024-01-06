/**
 * Il metodo check prende come parametri 2 elementi:
 *   1)array di interi
 *   2)dimensione dell'array
 * Ti restituisce -1 se non esiste un elemento all'interno del vettore con valore -1, nel caso
 * ci fosse un elemento pari a -1, restituisce la posizione dell'elemento all'interno del vettore
 * 
*/
int check(int *array, int size);

//metodo scissione
void scissione(int* nAtomico, int argc, char* argv[]);

/**
 * Il metodo aggiungiAtomo si occupa di aggiungere effettivamente il pid all'interno del vettore condiviso
 * Lo fa attraverso 2 modi:
 *   1) Aumenta la dimensione del vettore condiviso di 1 e occupa la nuova cella con il pid
 *   2) Se all'interno trova una cella che ha come elemento -1, allora modifica quella cella con il pid senza aumentare la dimensione
 * La scelta dei 2 modi dipende tutto dal valore restituito dal metodo check()
 * Il parametro passato è pid ed è il process Identifier del processo da inserire nel vettore
 * Infine si aggiorna il campo energiaTotale aggiungendo quella liberata
*/
void aggiungiAtomo(int pid, int energiaLiberata);

/**
 * Il metodo consiste nel eliminare il processo atomo che si è suidato all'interno del vettore condiviso dei pid
 * i parametri seguenti sono:
 * 1) Pid del processo atomo morto
 * 2) identificatore del vettore dei semafori
 * 3) un intero che identifica le scorie da aggiungere
 * 
 * Dopo aver eliminato il pid(mettendo a -1 la sua cella per segnare che è libera) si aggiorna il campo
 * scorieTotali  con il terzo parametro
*/
void removePid(int pid, int scorie);


int find(int elem,int * array,int n);

/*Operazione sul semaforo per decrementare*/
void V(int nSem);

/*Operazione sul semaforo per incrementare*/
void P(int nSem);