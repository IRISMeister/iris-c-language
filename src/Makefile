CC            = gcc
CFLAGS        = -g3 -O3 -I /usr/irissys/dev/iris-callin/include
LDFLAGS       = -L/usr/irissys/bin -Wl,-rpath,/usr/irissys/bin
LIBS          = -lirisdb
LIBSMT        = -lirisdbt
PROGRAM       = callin_misc callin_unicode_value_test callin_multithreads callout.so callin_error

all:            $(PROGRAM)

callin_misc:	callin_misc.o callin_misc_calls.o callin_execute.o
	$(CC) callin_misc.o callin_misc_calls.o callin_execute.o $(LDFLAGS) $(LIBS) -o callin_misc

callin_unicode_value_test:	callin_unicode_value_test.o
	$(CC) callin_unicode_value_test.o $(LDFLAGS) $(LIBS) -o callin_unicode_value_test

callin_multithreads:	callin_multithreads.o
	$(CC) callin_multithreads.o $(LDFLAGS) $(LIBSMT) -pthread -o callin_multithreads

callin_error:	callin_error.o
	$(CC) callin_error.o $(LDFLAGS) $(LIBS) -o callin_error

callout.so:	callout.o
	$(CC) callout.o -shared -o callout.so

callout.o:	callout.c
	$(CC) callout.c $(CFLAGS) -c -fPIC -o callout.o

clean:
	rm -f *.o *~ $(PROGRAM)
