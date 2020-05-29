echo "Compiling..."
gcc callin_misc.c callin_misc_calls.c -I /usr/irissys/dev/iris-callin/include -Wl,-rpath,/usr/irissys/bin -L/usr/irissys/bin -lirisdbt -o callin_misc -g3 -O0
gcc callin_multithreads.c -I /usr/irissys/dev/iris-callin/include -Wl,-rpath,/usr/irissys/bin -L/usr/irissys/bin -lirisdbt -pthread -o callin_multithreads -g3 -O0
gcc callin_unicode_value_test.c -I /usr/irissys/dev/iris-callin/include -Wl,-rpath,/usr/irissys/bin -L/usr/irissys/bin -lirisdbt -o callin_unicode_value_test -g3 -O0

gcc callout.c -fPIC -I /usr/irissys/dev/iris-callin/include -shared -o callout.so -g3 -O0

