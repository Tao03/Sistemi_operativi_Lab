#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// il metodo consiste nel scegliere casualmente il processo atomo vittima
void scegliAtomoVittima();