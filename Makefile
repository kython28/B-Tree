main:
	gcc -O2 -c src/btree.c -o btree.o

probe: main
	gcc -O2 test/probe.c btree.o -o probe