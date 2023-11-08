---
title: Appunti di Sistemi operativi laboratorio
author: Taoufik
date: today
...
# Processi

Il comando " kill " invia un segnale al processo, un esempio può essere SIGKILL (aggiungendo un PID) porta
alla terminazione del processo.
Il comando kill può essere usato anche per mettere in pausa il processo oppure farlo continuare.

Siccome tutti i processi sono identificati da un PID e ha un processo Padre che gli ha generati, le system call:
- getpid() : consiste nel restituire il PID del processo corrente </li>
- gettpid() : consiste nel restituire il PID del processo padre </li>

### Comandi utili nel terminale</h3>

- **ps -Af | less**: Mostra tutte le informazioni di tutti i processi attivi 
-  **pstree -p | less** : Visualizza l'albero gerarchico dei processi attivi
### Creazione di un processo
L'istruzione fork() è una system call che **consiste al processo chiamato ora padre di creare un processo figlio**
```C 
#include < unistd.h >
pid_t fork ( void );
```
Con questa porzione di codice il sistema operativo duplica il processo padre, quindi il processo figlio appena generato avrà le stesse informazioni
del padre e verrà eseguito nuovamente\

**ATTENZIONE:** Siccome sono aree di memoria condivise, il processo padre non nota che il figlio modifichi le informazioni al momento dell'esecuzione
<p> Un altro aspetto interessante è che la funzione restituisce 2 valori distinti:\

- Nel processo Padre, il PID del figlio
- Nel processo figlio, 0

### Differenziare processo padre e figlio
L'unico modo per differenziare l'esecuzione dei 2 processi è attraverso il seguente codice:
   
```C
/* Executed only once */
if ( fork () ) {
    /* Executed by parent only */
} else {
    /* Executed by child only */
}
/* Executed twice : by both parent and child */
```


Il valore che ritorna la funzione ci dice che processo stiamo eseguendo\
# Primo approccio di sincronizzazione tra processi
### Il metodo Waiting
Un processo padre può aspettare la terminazione del processo figlio attraverso un modo di sincronizzazione nominato **waiting**<br>
Per esempio il comando può essere usato in ben 2 casi:
> **gedit**: <br>
Consiste nell'eseguire gedit che è un processo figlio e aspetta fino a quando il processo gedit non termini.
<hr>

> **gedit &** : 
Consiste nell'eseguire gedit che è un processo figlio, ma in questo caso non aspetta la terminazione del processo figlio e va avanti creando altri processi.

### Aspettando la terminazione con wait in C
Se si vuole implementare questo semplice meccanismo di sincronizzazione, basta saper utilizzare il metodo 
> **pid_t wait(NULL)**
In questo caso il metodo può restituire un valore che assume 2 significati diversi:
1. Nel caso fosse uguale a -1 significa che tutti i processi figli hanno terminato l'esecuzione
2. Nel caso fosse diverso da -1 significa che il processo che ha come pid il valore restituito ha terminato la sua esecuzione.

**Nota bene**: Questa tecnica consiste nel chiamare una **system call** che blocca il processo, può sembrare una tecnica scomoda ma è molto comoda e usata quando il processo padre deve aspettare la terminazione dei processi figli\
### A cosa succede se i processi genitori terminano prima?
Non è consigliabile questa situazione e difatti si utilizza il primo metodo di sincronizzazione per evitare questa possibilità.\
Tuttavia può capitare che il **processo figlio termini prima di una wait di un processo padre**: le risorse richieste al processo figlio vengono rilasciate ma **la riga della tabella dei processi associata al figlio rimane** e quindi diventa un processo zombie\
Per risolvere questo problema basta utilizzare un'altra wait, così il processo padre si accorge della terminazione del processo figlio

### Terminazione di un processo e lo stato di uscita
Ci sono 2 motivo per cui un processo termina:
1. Interno, quindi il processo decide di terminare attraverso una **return** oppure una **system call**
2. Esterno, attraverso un segnale che porti alla terminazione il processo come il comando KILL nel terminale, si chiama esterno siccome non è il processo a decidere di terminare ma enti esterni.

Se si vuole aspettare un determinato processo figlio allora si può optare per l'istruzione
>pid_t waitpid ( pid_t pid , int * status_child , int options ).

Dopo aver chiamato la funzione, il processo aspetta fino a quando il processo desiderato **ha terminato la sua esecuzione**. Nel caso il valore restituito è -1 allora c'è stato un errore e attraverso la macro errno si può definire il tipo di errore, altrimenti restituisce il pid del processo figlio che si aspettava.
# Segnali
I segnali possono essere generati da un utente, software oppure anche hardware\
Alcuni esempi di segnali possono essere:
1. SIGKILL: causa una terminazione del programma
2. SIGSTOP: ferma/avanza l'esecuzione di un processo

Per ottenere informazioni più approfondite basta usare il comando:
> man 7 signal

### Invio di segnali ad un qualsiasi processo
Il seguente pezzo di codice..
```C
# include < sys/types.h >
# include < signal.h >

int kill ( pid_t pid , int signum );
```
Ci dice che attraverso la procedura kill si può uccidere un processo utilizzando 2 parametri:
1. **pid**: codice univoco che identifica il processo da uccidere(se inseriamo -1 come pid allora si invia un segnale a tutti i processi esistenti)
2. **signum**:  il tipo di segnale che si vuole inviare ad un processo (se è pari a 0 allora si fa un test sull'esistenza del PID)

Invece se si vuole inviare un segnale ad un processo dalla comand line, basta farlo con il seguente comando:\
>kill -(signal) (pid)

**NB: se non si specifica il segnale, per default è SIGTERM**

### Invio di segnali a se stessi
Il codice di pezzo seguente ci permette di inviare un segnale di tipo **SIGNUM** a noi stessi
```C
# include <signal.h>
int raise ( int signum ) ;
```
Oppure possiamo chiedere al sistema operativo di inviare inserire un allarme dop sec secondi:
```C
#include < unistd .h >
unsigned int alarm (unsigned int sec) ;
```
**NB: la funzione alarm restituisce il tempo rimanente all'allarme nel caso il processo corrente dovesse essere interrotto**
### Gestione di segnali da parte del processo destinatario
Ogni segnale ha un suo handler predefinito che sono i seguenti:
1. **Term**: termina il processo
2. **Ign**: ignora il segnale
3. **Core**: il processo viene terminato ma viene salvata l'immagine del processo, quindi tutte le informazioni relative allo stato del processo.
Viene usato per fare debugging
4. **Stop**: ferma il processo
5. **Cont**: Continua il processo nel caso si fosse fermato.

Un handler predefinito a livello utente viene richiamato in modo **asincrono => ovvero che viene attivato subito**\
**indipendentemente da quello che sta succedendo**. Subito dopo, accadono 3 cose fondamentali:
1. Lo stato del processo viene salvato
2. La funzione(handler) viene eseguita
3. lo stato del processo precedente viene caricato.

**NB: lo handler dei segnali è simile a quello degli interrupt e alcuni segnali non possono essere gestiti dagli hanlder "utente"**\
E' pure possibile gestire i segnali pure in modo sincrono **ma non viene spiegato in questo corso**

### Implementazione di un handler a livello utente
Per un implementazione della gestione effettiva dei segnali si deve utilizzare una struttura del tipo:
```C
struct sigaction{
    void (* sa_handler )( int signum ) ;
    sigset_t sa_mask ; /* illustrated later */
    int sa_flags ; /* illustrated later */
    /* plus others ( for advanced users ) */
};
```
**sa_handler** è un puntatore ad una funziona definita come 
>void signal_handler ( int signum );
che viene invocata appena si riceve il segnale
**NB: Si ricorda che si deve impostare la struct a 0**
```C
# define _GNU_SOURCE /* necessary from now on */
# include < signal .h >
int sigaction ( int signum , const struct sigaction * act ,
struct sigaction * oldact );
```
Come si può notare dal codice, il metodo **sigaction** serve a impostare una qualsiasi signal handler attraverso vari parametri:
1. **signum**: indica che tipo di segnale si voglia 
2. **act**: indica il signal handler nuovo
3. **oldact**: indica il signal handler vecchio

#### Esempio di codice che gestisce un segnale
```C
void handle_signal ( int signal ); /* the handler */
struct sigaction new , old ;
bero (& new , sizeof ( new )); /* set all bytes to zero */
new . sa_handler = handle_signal ; /* set the handler */
sigaction ( signum ,& new , NULL ); /* CASE 1: set new handler */
sigaction ( signum , NULL ,& old ); /* CASE 2: get cur handler */
sigaction ( signum ,& new ,& old );
```

Una semplice implementazione di un handler sarebbe attraverso uno switch case dove si seleziona, attravero il valore del **SIGNUM** il suo handler corrispondente
```C
void handle_signal ( int signum ) {
/* signal signum triggered the handler */
switch ( signum ) {
case SIGINT :
/* handle SIGINT */
break ;
case SIGALRM :
/* handle SIGALRM */
break ;
/* other signals */ } }
```

Altre informazioni utili possono essere:
1. Gli handler di un processo padre vengono ereditati a tutti i processi figlio
2. gli hanlder vengono puliti dopo il comando **execve**
3. Le variabili globali vengono riconosciute anche all'interno del handler e offrono una sorta di comunicazione al main siccome attraverso le variabili globali il main può vedere quello che sta accadendo al'interno degli handler.
Tuttavia si deve prestare molta attenzione all'uso di variabili/strutture globali

### Differenza tra una funziona safe e no
Quando si parla di segnali e handler, le funzioni di libreria si suddividono in 2 categorie:
1. **Asynchronous Signal-Safe**: funzioni che non utilizzano variabili globali
2. **Asynchronous Signal-UnSafe**: funzioni che utilizzando variabili globali e definite non sicure, per esempio **printf(...)** non lo è.

Quind ogni volta che si vuole aggiungere una funzione di libreria in una handler, si deve prima assicurarsi che sia AS-SAFE ovvero **Asynchronous Signal-Safe**
## Lifecycle of signals: delivering, masking, merging
I segnali vengono generati via **hardware/software** e possono essere posticipati attraverso una **maschera**.\
Se un processo è mascherato, il segnale appena generato rimarrà nello stato di **pending**.\
Appena si toglie la maschera, il processo riceve il segnale.
Nel caso di 2 o più segnali venissero generati mentre il processo è mascherato\
viene effettuato il merge e appena il processo non è mascherato, il segnale (che è risultante di tutti i segnali precedenti) viene spedito al processo.

### Configurare una signal mask
Durante l'esecuzione, ogni processo ha una sua **signal mask**.\
Nel caso di una fork(), un processo figlio eredita la signal mask del padre, una signal mask è una collezione di tutti i segnali(si parla di 64 bit dove ogni bit è associato ad un segnale) e può essere modificata attraverso la system call:
>sigprocmask(...)

In c, esiste il tipo **sigset_t** che indica una struct di tipo maschera, le funzioni per lavorare con esse sono le seguenti:
```C
int sigemptyset ( sigset_t * set ) ;
int sigfillset ( sigset_t * set ) ;
int sigaddset ( sigset_t * set , int signum ) ;
int sigdelset ( sigset_t * set , int signum ) ;
int sigismember ( const sigset_t * set , int signum ) ;
```

Un modo per impostare una signal mask al processo corrente è quello di usare la\funzione **sigprocmask()**, nel seguente blocco di codice si vedere il numero e i tipi di parametri richiesti:
```C
# include < signal .h >
int sigprocmask ( int how , const sigset_t * set ,
sigset_t * oldset ) ;
```
1. oldset è la maschera vecchia
2. la maschera però può essere messa in vari modi:
    1. SIG_BLOCK: i segnali presenti nell'insieme dei segnali vengono aggiunti alla maschera corrente
    2. SIG_UNBLOCK: i segnali presenti nell'insieme vengono rimossi dalla maschera corrente
    3. SIG_SETMASK

Appena il segnale che viene inviato, lo handler lo maschera.
Il tutto viene fatto attraverso il campo **sa_handler** della sigaction (**VISTO LEZIONE SCORSA**): 
```C
/* How to mask a signal during SIGINT handler */
struct sigaction sa ;
sigset_t my_mask ;
bzero (& sa , sizeof (sa)) ; /* clean sa struct */
sa . sa_handler = handle_signal ; /* set handler */
sigemptyset (& my_mask ) ; /* Set an empty mask */
/* Add a signal to the sa_mask field struct sa */
sigaddset (& my_mask , signal_to_mask_in_handler ) ;
sa . sa_mask = my_mask ;
/* Set the handler */
sigaction ( SIGINT , & sa , NULL ) ;
```

### Segnali accorpati 
Nel caso un segnale venisse inviato ad un processo mascherato, il segnale va in stato di pendling e nel caso dovesse esserci un altro segnale, quest'ultimo viene "fuso" con il primo.
Questo perchè la presenza di un segnale viene registrata attraverso un valore booleano e non con un valore intero.

### Permettere l'esecuzione dei segnali durante la gestione di un altro segnale
Normalmente, quando viene inviato un segnale ad un processo che sta eseguendo un signal handler, il segnale nuovo viene bloccato fino a quando non finisce lo handler precedente.\
Tuttavia si può rendere questo sistema più flesssibile modificando il campo **sa_flags** come si vede nell'esempio:
```C
bzero (& sa , sizeof ( sa ) ) ;
sa.sa_handler = handle_signal ;
sa.sa_flags = SA_NODEFER ; /* nested signals */
sigaction ( SIGUSR1 , & sa , NULL ) ;
```
In parole povere vuole dire semplicemente che il processo gestisce il segnale anche se sta gestendo un segnale più vecchio(questo viene interotto momentaneamente)

### Generare un segnale ad un processo che è in stato di waiting
Ci sono 3 modi per portare un processo in stato di **wait**:
1. pause():
    ```C
    bzero (& sa , sizeof ( sa ) ) ;
    sa.sa_handler = handle_signal ;
    sa.sa_flags = SA_NODEFER ; /* nested signals */
    sigaction ( SIGUSR1 , & sa , NULL ) ;
    ```
    il processo rimane in stato di wait fino a quando non riceve un segnale

2. sleep(sec):
    ```C
    #include < unistd .h >
    unsigned int sleep ( unsigned int seconds ) ;
    ```
    Il processo si mette in stato di wait per un tot di secondi, se il processo riceve un segnale mentre è in "sleeping", si sveglia e la funzione restituisce il tempo rimanente per cui il processo deve ancora dormire.\
    **NOTA BENE: NON USARE SLEEP NEL PROGETTO, E' UNA PESSIMA IDEA IN GENERALE**

3. nanosleep():
    ```C
    #include < time .h >
    struct timespec my_time ;
    my_time . tv_sec = 1;
    my_time . tv_nsec = 234567000;
    nanosleep (& my_time , NULL ) ;
    ```
    E' la miglior alternativa allo sleep()

### Pessimo modo di sincronizzare i processi
Un modo **SBAGLIATO** di sincronizzare l'esecizione dei processi è quello di usare sleep() o nanosleep() e lo si può notare guardando il seguente blocco di codice:
```C
if ( fork () ) {
/* PARENT */
sleep (10) ;
/* the parent thinks the child has finished */
} else {
/* CHILD */
/* do my work before the parent checks */
}
```
E' sbagliato per 2 semplici motivi:
Innanzitutto noi non sappiamo se il tempo di esecuzione del processo figlio sia minore o uguale a 10 secondi.
Il secondo motivo è che il processo figlio potrebbe non essere schedulato per più di 10 secondi, in questo caso non ha neanche iniziato.\
Quindi possiamo dire che:
- sleep() garantisce che il processo dorma per un tot di secondi
- sleep()/nanosleep() vengono usati per operazioni di outptu
- **MAI USARE** sleep() o nanosleep() per aspettare un altro processo(come nell'esempio precedente)

Cosa succede quando un processo viene messo in pausa(attraverso una system call) con **wait() pause() sleep()**?
- Lo stato del processo viene salvato
- la funzione di handler viene eseguita
- dopo che la funzione di handler è stata restituita, possono esserci 2 casi:
    1. restarting: quando viene restituito un valore dal handler, la systema call viene rifatta 
    2. abort: la system call viene eliminata
Il comportamento predefinito è il **secondo, ovvero aborting** ma si può cambiare il comportantemento attraverso il campo **SA_RESTART** che si trova nella struct **sigaction**, sfortunatamente diverse system call hanno comportamenti diversi e conviene (nel caso si volesse operare con le system call) approfondire l'argomento attraverso il terminale inserendo il comando:
> man 7 signal


