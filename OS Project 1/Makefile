all: project1

storage.o: storage.c
	gcc -c storage.c

project1.o: project1.c
	gcc -c project1.c 

project1: project1.o storage.o
	gcc project1.o storage.o -o project1

clean:
	rm -f project1 *.o

zip:
	zip project1.zip *.c *h  Makefile README.txt
