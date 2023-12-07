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
#include "master.h"
void setSemaforo()
{
    key_t key = 1234;
    int id=semget(key, 3, IPC_CREAT);
    semctl(id, 0, SETVAL, 1);

    
}
