supports = oufs_lib.o oufs_lib_support.o virtual_disk.o storage.o

all: oufs_format oufs_ls oufs_mkdir oufs_rmdir oufs_stats oufs_inspect


storage.o: storage.c
	gcc -c storage.c

virutal_disk.o: virtual_disk.c
	gcc -c virtual_disk.c

oufs_lib.o: oufs_lib.c 
	gcc -c oufs_lib.c

oufs_lib_support.o: oufs_lib_support.c
	gcc -c oufs_lib_support.c



oufs_format.o: oufs_format.c
	gcc -c oufs_format.c

oufs_format: oufs_format.o $(supports)
	gcc oufs_format.o $(supports) -o oufs_format

oufs_ls.o: oufs_ls.c
	gcc -c oufs_ls.c

oufs_ls: oufs_ls.o $(supports)
	gcc oufs_ls.o $(supports) -o oufs_ls

oufs_mkdir.o: oufs_mkdir.c
	gcc -c oufs_mkdir.c

oufs_mkdir: oufs_mkdir.o $(supports)
	gcc oufs_mkdir.o $(supports) -o oufs_mkdir

oufs_rmdir.o: oufs_rmdir.c
	gcc -c oufs_rmdir.c

oufs_rmdir: oufs_rmdir.o $(supports)
	gcc oufs_rmdir.o $(supports) -o oufs_rmdir

oufs_inspect.o: oufs_inspect.c
	gcc -c oufs_inspect.c

oufs_inspect: oufs_inspect.o $(supports)
	gcc oufs_inspect.o $(supports) -o oufs_inspect

oufs_stats.o: oufs_stats.c
	gcc -c oufs_stats.c

oufs_stats: oufs_stats.o
	gcc oufs_stats.o -o oufs_stats



zip:
	zip project3.zip *.c *.h Makefile README.txt

clean:
	rm -f oufs_format oufs_ls oufs_mkdir oufs_rmdir oufs_stats oufs_inspect *.o
