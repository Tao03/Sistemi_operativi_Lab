all: Master Attivatore Atomo Alimentatore Allarme Inibitore
Master: Master.c Modules/master-module.c
	gcc -c Modules/master-module.c -o Modules/master-module.o
	gcc Master.c Modules/master-module.o -o Master
Attivatore: Attivatore.c Modules/attivatore-module.c
	gcc -c Modules/attivatore-module.c -o Modules/attivatore-module.o
	gcc Attivatore.c Modules/attivatore-module.o -o Attivatore

Atomo: Atomo.c Modules/atomo-module.c
	gcc -c Modules/atomo-module.c -o Modules/atomo-module.o
	gcc Atomo.c Modules/atomo-module.o -o Atomo

Alimentatore: Alimentatore.c Modules/alimentatore-module.c
	gcc -c Modules/alimentatore-module.c -o Modules/alimentatore-module.o
	gcc Alimentatore.c Modules/alimentatore-module.o -o Alimentatore

Allarme: Alarm.c
	gcc Alarm.c -o Alarm
Inibitore: Inibitore.c
	gcc Inibitore.c -o Inibitore

clean:
	rm -f *.o Master Attivatore Atomo Alimentatore Inibitore Alarm
#gcc -c Modules/master-module.c
#gcc Master.c Modules/master-module.o -o Master