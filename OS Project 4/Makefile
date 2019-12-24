supports = oufs_lib.o oufs_lib_support.o virtual_disk.o storage.o

everything = oufs_format oufs_ls oufs_mkdir oufs_rmdir oufs_stats oufs_inspect oufs_touch oufs_create oufs_append oufs_cat oufs_remove oufs_link oufs_copy

all : $(everything)


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



oufs_touch.o: oufs_touch.c
	gcc -c oufs_touch.c

oufs_touch: oufs_touch.o $(supports)
	gcc oufs_touch.o $(supports) -o oufs_touch

oufs_create.o: oufs_create.c
	gcc -c oufs_create.c

oufs_create: oufs_create.o $(supports)
	gcc oufs_create.o $(supports) -o oufs_create

oufs_append.o: oufs_append.c
	gcc -c oufs_append.c

oufs_append: oufs_append.o $(supports)
	gcc oufs_append.o $(supports) -o oufs_append

oufs_cat.o: oufs_cat.c
	gcc -c oufs_cat.c

oufs_cat: oufs_cat.o $(supports)
	gcc oufs_cat.o $(supports) -o oufs_cat

oufs_copy.o: oufs_copy.c
	gcc -c oufs_copy.c

oufs_copy: oufs_copy.o $(supports)
	gcc oufs_copy.o $(supports) -o oufs_copy

oufs_remove.o: oufs_remove.c
	gcc -c oufs_remove.c

oufs_remove: oufs_remove.o $(supports)
	gcc oufs_remove.o $(supports) -o oufs_remove

oufs_link.o: oufs_link.c
	gcc -c oufs_link.c

oufs_link: oufs_link.o $(supports)
	gcc oufs_link.o $(supports) -o oufs_link


zip:
	zip project4.zip *.c *.h Makefile README.txt

clean:
	rm -f $(everything) *.o
