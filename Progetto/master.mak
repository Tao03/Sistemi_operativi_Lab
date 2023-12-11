Master: libmodules.a Master.o
    gcc Master.o -lmodules -L. -o Master
libmodules.a: master-module.o alimentatore-module.o atomo-module.o attivatore-module.o
    ar -crs libmodules.a master-module.o alimentatore-module.o atomo-module.o attivatore-module.o
master-module.o: Modules/master-module.c
    gcc -c Modules/master-module.c
alimentatore-module.o: Modules/alimentatore-module.c
    gcc -c Modules/alimentatore-module.c
atomo-module.o: Modules/atomo-module.c
    gcc -c Modules/atomo-module.c
attivatore-module.o: Modules/attivatore-module.c
    gcc -c Modules/attivatore-module.c
Master.o: Master.c
    gcc -c Master.c