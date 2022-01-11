all: saturnd cassini

saturnd: saturnd.o execute-request.o string2.o timing-text-io.o commandline.o task.o write-data.o read-data.o open-pipe.o run.o
	gcc saturnd.o execute-request.o string2.o timing-text-io.o commandline.o task.o write-data.o read-data.o open-pipe.o run.o -o saturnd

cassini: cassini.o commandline.o string2.o timing-text-io.o write-data.o read-data.o open-pipe.o
	gcc cassini.o commandline.o string2.o timing-text-io.o write-data.o read-data.o open-pipe.o -o cassini

cassini.o: src/cassini.c
	gcc -c -Iinclude -Wall src/cassini.c

saturnd.o: src/saturnd.c
	gcc -c -Iinclude -Wall src/saturnd.c

execute-request.o: src/execute-request.c
	gcc -c -Iinclude -Wall src/execute-request.c

task.o: src/task.c
	gcc -c -Iinclude -Wall src/task.c

commandline.o: src/commandline.c
	gcc -c -Iinclude -Wall src/commandline.c

string2.o: src/string2.c
	gcc -c -Iinclude -Wall src/string2.c

timing-text-io.o: src/timing-text-io.c
	gcc -c -Iinclude -Wall src/timing-text-io.c

write-data.o: src/write-data.c
	gcc -c -Iinclude -Wall src/write-data.c

read-data.o: src/read-data.c
	gcc -c -Iinclude -Wall src/read-data.c

open-pipe.o: src/open-pipe.c
	gcc -c -Iinclude -Wall src/open-pipe.c

run.o: src/run.c
	gcc -c -Iinclude -Wall src/run.c

distclean:
	rm -f *.o cassini saturnd
