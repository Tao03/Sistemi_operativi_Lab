#define _GNU_SOURCE
#define KEY_SEMAFORO 1234
#define KEY_MEMORIA_CONDIVISA 1111
#define KEY_ARRAY_CONDIVISO 1221
#define N_ATOMI_INIT  1
#define ENERGY_DEMAND 100
#define N_ATOMO_MAX 100
#define SIM_DURATION 100
#define ENERGY_EXPLODE_THRESHOLD 1500
#define N_ATOMICO_MIN 25 //numero atomico minimo per fare la scissione
#define N_ATOMICO_MAX 100 //numero atomico massimo che può avere un atomo

#define TIMER_ALIMENTATORE 3
#define TIMER_PRELEVA 1
#define TIMER_ATTIVATORE 2




#ifndef STRUCT
#define STRUCT
extern struct memCond
    {
        int id_vettore_condiviso;  // vettore dei pid degli atomi
        int nAtomi; // grandezza del vettore
        int eTot;   // energia totale sprigionata
        int nScissioni; //numero delle scissioni effettive
        int nAttivazioni; // quante volte l'attivatore ha eseguito il suo compito
        int eConsumata; // energia consumata dal reattore
        int scorie; // scorie prodotte dal reattore
        int pidInibitore; //pid del processo inibitore

    }dummy;// è solo per la creazione della memoria condivisa
#endif