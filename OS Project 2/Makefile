all: project2 server

pipe_in:
	mkfifo pipe_in

pipe_out:
	mkfifo pipe_out

pipes: pipe_in pipe_out

storage.o: storage.c
	gcc -c storage.c

storage_remote.o: storage_remote.c
	gcc -c storage_remote.c

project2.o: project2.c
	gcc -c project2.c

project2: project2.o storage_remote.o
	gcc project2.o storage_remote.o -o project2

server.o: server.c
	gcc -c server.c

server: server.o storage.o
	gcc server.o storage.o -o server

clean:
	rm -f project2 server pipe_in pipe_out *.o

zip:
	zip project2.zip *.c *.h Makefile README.txt
