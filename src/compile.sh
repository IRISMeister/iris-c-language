gcc callin.c -I /usr/irissys/dev/iris-callin/include -Wl,-rpath,/usr/irissys/bin -L/usr/irissys/bin -lirisdbt -o callin -g3 -O0
gcc callint.c -I /usr/irissys/dev/iris-callin/include -Wl,-rpath,/usr/irissys/bin -L/usr/irissys/bin -lirisdbt -pthread -o callint -g3 -O0
gcc callout.c -fPIC -I /usr/irissys/dev/iris-callin/include -shared -o callout.so -g3 -O0
