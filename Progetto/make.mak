Master: Modules/master-module.c Modules/master-module.o Master.c
	gcc -c Modules/attivatore-module.c
	gcc Master.c Modules/master-module.o -o Master

#master-module.o: Modules/master-module.c
#	gcc -c Modules/master-module.c
#
#Master.o: Master.c
#	gcc -c Master.c
#
#gcc -c Modules/master-module.c
#gcc Master.c Modules/master-module.o -o Master