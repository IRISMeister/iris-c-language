gcc callintest.c -I /usr/irissys/dev/iris-callin/include -Wl,-rpath,/usr/irissys/bin -L/usr/irissys/bin -lirisdbt -o callintest -g3 -O0
gcc callout.c -fPIC -I /usr/irissys/dev/iris-callin/include -shared -o callout.so -g3 -O0
