CC            = gcc
CFLAGS        = -g3 -O3 -I /usr/irissys/dev/iris-callin/include
LDFLAGS       = -L/usr/irissys/bin -Wl,-rpath,/usr/irissys/bin
LIBS          = -lirisdbt
PROGRAM       = callin_misc callin_unicode_value_test callin_multithreads callout.so

all:            $(PROGRAM)

callin_misc:	callin_misc.o callin_misc_calls.o
	$(CC) callin_misc.o callin_misc_calls.o $(LDFLAGS) $(LIBS) -o callin_misc

callin_unicode_value_test:	callin_unicode_value_test.o
	$(CC) callin_unicode_value_test.o $(LDFLAGS) $(LIBS) -o callin_unicode_value_test

callin_multithreads:	callin_multithreads.o
	$(CC) callin_multithreads.o $(LDFLAGS) $(LIBS) -pthread -o callin_multithreads

callout.so:	callout.o
	$(CC) callout.o -shared -o callout.so

callout.o:	callout.c
	$(CC) callout.c $(CFLAGS) -c -fPIC -o callout.o

clean:
	rm -f *.o *~ $(PROGRAM)
