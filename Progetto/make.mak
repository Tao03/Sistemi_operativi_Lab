all: Master Attivatore Atomo Alimentatore Allarme Inibitore
Master: Master.c Modules/master-module.c
	gcc -c Modules/master-module.c
	gcc Master.c master-module.o -o Master
Attivatore: Attivatore.c Modules/attivatore-module.c
	gcc -c Modules/attivatore-module.c
	gcc Attivatore.c attivatore-module.o -o Attivatore

Atomo: Atomo.c Modules/atomo-module.c
	gcc -c Modules/atomo-module.c
	gcc Atomo.c atomo-module.o -o Atomo

Alimentatore: Alimentatore.c Modules/alimentatore-module.c
	gcc -c Modules/alimentatore-module.c
	gcc Alimentatore.c alimentatore-module.o -o Alimentatore

Allarme: Alarm.c
	gcc Alarm.c -o Alarm
Inibitore: Inibitore.c
	gcc Inibitore.c -o Inibitore

clean:
	rm -f *.o Master Attivatore Atomo Alimentatore
#gcc -c Modules/master-module.c
#gcc Master.c Modules/master-module.o -o Master