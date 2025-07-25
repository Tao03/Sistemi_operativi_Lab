#define _GNU_SOURCE
#define KEY_SEMAFORO 1234
#define KEY_MEMORIA_CONDIVISA 1111
#define KEY_ARRAY_CONDIVISO 1221
#define KEY_CODA_MESSAGGI 1331
#define N_ATOMI_INIT  1000
#define ENERGY_DEMAND 2000
#define ENERGY_CONSUMPTION 100
#define N_ATOMO_MAX 100
#define SIM_DURATION 100
#define ENERGY_EXPLODE_THRESHOLD 10000
#define N_ATOMICO_MIN 25 //numero atomico minimo per fare la scissione
#define N_ATOMICO_MAX 100 //numero atomico massimo che può avere un atomo
#define N_NUOVI_ATOMI 5
#define STEP_ALIMENTAZIONE  999999999
#define TIMER_PRELEVA 1
#define STEP_ATTIVATORE 2




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
        int scorieUtilmoSecondo;
        int nAttivazioniUltimoSecondo;
        int nScissioniUltimoSecondo;
        int eTotUltimoSecondo;
        int energiaAssorbita;

    }dummy;// è solo per la creazione della memoria condivisa

extern struct messaggio {
    long tipo;
    int energia;
    int esito;
}dummy1;
#endif