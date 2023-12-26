//metodo ausiliario 
int check(int *array, int size);
//metodo scissione
void scissione(int* nAtomico, int argc, char* argv[]);
//metodo che aggiunge un atomo al vettore condiviso
void aggiungiAtomo(int pid, int energiaLiberata);
void removePid(int pid,int idSemaforo, int scorie);
int find(int elem,int * array,int n);