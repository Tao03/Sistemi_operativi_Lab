#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

//metodo per creare e inizializzare il vettore dei semafori, restituisce l'identificatore dell'array dei semafori
int setSemaforo();

/**
 * Il metodo consiste nel creare 2 segmenti di memoria condivisi a tutti i processi
 * Il primo serve a tenere traccia delle informazioni relative alle statisiche come numero scissioni, energia totale ecc.
 * Invece il secondo ci serve per tenere traccia dei Processe Identifier dei processi atomo
 * Si restituisce l'identificatore del primo segmento di memoria
*/
int setMemoriaCondivisa();

//metodo per visualizzare nel terminale le statistiche
void stampa();

//metodo per creare i processi atomo
void creaAtomi( int nAtomoMax, int idMemoriaCondivisa);

/**
 * Il metodo consiste nel prelevare una quantita definita dal parametro energyDemand di energia 
*/
void prelevaEnergia(int energia);

void terminazione(int idSemaforo,int idMemoriaCondivisa,int check,int tempoScaduto,int forkError, int pidAlimentatore, int pidAttivatore,int pidInibitore, char* argv[],int argc);


/**
 * Dato i 3 parametri, il metodo consiste nel registrare il Process identifier del processo atomo nell'array condiviso:
 * 
 * 1)indice: indica la posizione da inserire all'interno del vettore
 * 2)pid: il valore effettivo che deve essere inserito
 * 3)idMemoriaCondivisa: l'identificatore della memoria condivisa
*/
void insertAtomi(int indice, int pid, int idMemoriaCondivisa);

/**
 * Il metodo consiste nel controllare se la simulazione è andata in EXPLODE oppure in BLACKOUT
*/
int checkEnergia();

/**
 * Metodo che consiste nel inserire il pid dell'inibitore nella memoria condivisa
*/
void inserisciInibitore(int pidInibitore);

/**
 * Il metodo consiste nel visualizzare le statistiche richieste nella consegna
*/
void stampa();

/*Operazione sul semaforo per decrementare*/
void V(int nSem);

/*Operazione sul semaforo per incrementare*/
void P(int nSem);