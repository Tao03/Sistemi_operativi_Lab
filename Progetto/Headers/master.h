#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

//metodo per inizializzare il semaforo normale
int setSemaforo();
//metodo per inizializzare la memoria condivisa
int setMemoriaCondivisa(int nKids);
//metodo per stampare le statistiche
void getStatistiche();
//metodo per creare i processi atomo
void creaAtomi(int nKids, int nAtomoMax, int idMemoriaCondivisa);
//metodo per prelevare energia
void prelevaEnergia(int energia);

void stampa();

void insertAtomi(int indice, int pid, int idMemoriaCondivisa);


int checkEnergia();

void stampa();