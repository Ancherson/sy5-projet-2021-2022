cassini: cassini.o commandline.o string2.o timing-text-io.o
	gcc cassini.o commandline.o string2.o timing-text-io.o -o cassini

cassini.o: src/cassini.c
	gcc -c -Iinclude -Wall src/cassini.c

commandline.o: src/commandline.c
	gcc -c -Iinclude -Wall src/commandline.c

string2.o: src/string2.c
	gcc -c -Iinclude -Wall src/string2.c

timing-text-io.o:
	gcc -c -Iinclude -Wall src/timing-text-io.c

distclean:
	rm -f *.o cassini

