#define KEY_SEMAFORO 1234
#define KEY_MEMORIA_CONDIVISA 1111
#define N_ATOMI_INIT  2
#define N_ATOMO_MAX 100
#ifndef STRUCT
#define STRUCT
extern struct memCond
    {
        int *vPid;  // vettore dei pid degli atomi
        int nAtomi; // grandezza del vettore
        int eTot;   // energia totale sprigionata

    }dummy;// è solo per la creazione della memoria condivisa
#endif