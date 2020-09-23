CC            = gcc
#CFLAGS       = -O1 -I /usr/irissys/dev/iris-callin/include
CFLAGS        = -g3 -ggdb -O3 -I /usr/irissys/dev/iris-callin/include
LDFLAGS       = -L/usr/irissys/bin -Wl,-rpath,/usr/irissys/bin
LIBS          = -lirisdb
LIBSMT        = -lirisdbt
PROGRAM       = callin_repeat callin_misc callin_multithreads callout.so 

all:            $(PROGRAM)

callin_repeat:	callin_repeat.o callin_misc_calls.o shdir.o
	$(CC) callin_repeat.o callin_misc_calls.o shdir.o $(LDFLAGS) $(LIBSMT) -o callin_repeat

callin_misc:	callin_misc.o callin_misc_calls.o callin_execute.o callin_misc_value.o shdir.o
	$(CC) callin_misc.o callin_misc_calls.o callin_execute.o callin_misc_value.o shdir.o $(LDFLAGS) $(LIBSMT) -o callin_misc

callin_multithreads:	callin_multithreads.o shdir.o
	$(CC) callin_multithreads.o shdir.o $(LDFLAGS) $(LIBSMT) -pthread -o callin_multithreads

callout.so:	callout.o
	$(CC) callout.o -shared -o callout.so

callout.o:	callout.c
	$(CC) callout.c $(CFLAGS) -c -fPIC -o callout.o

clean:
	rm -f *.o *~ $(PROGRAM)
