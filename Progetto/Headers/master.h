#define _GNU_SOURCE
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
//metodo per inizializzare il semaforo normale
void setSemaforo();
//metodo per inizializzare la memoria condivisa
void setMemoriaCondivisa();
//metodo per stampare le statistiche
void getStatistiche();
//metodo per creare i processi atomo
void creaAtomi(int nKids);
//metodo per prelevare energia
void prelevaEnergia();