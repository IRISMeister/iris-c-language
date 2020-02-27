The simpest example of how to build/use IRIS Callin and Callout in Linux.  
Callin (callintest) will get value from ^test global and set ^test2(1) to ^test2(10) global nodes.  
Callout (callout.so) will add two given integres and returns it.  
# How to RUN

```bash
user@host:~/iris-c-lang$ docker-compose up -d
user@host:~/iris-c-lang$ docker-compose exec iris bash
irisowner@d6be47c31b1b:~$ cd /opt/iris/
irisowner@d6be47c31b1b:/opt/iris$ ./callintest
rc:0
IrisSecureStartA Status :Success.
IrisPushGlobal rc:0
IrisGlobalGet rc:0
IrisPopStr rc:0
value:abc
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisGlobalGet rc:0
IrisPopStr rc:0
value:2
    ...
IrisGlobalSet rc:0
Exiting.
irisowner@d6be47c31b1b:/opt/iris$
irisowner@d6be47c31b1b:/opt/iris$ iris session iris

ノード: d6be47c31b1b インスタンス: IRIS
```
```ObjectScript
USER>zw ^test2
^test2(1)="abc"
^test2(2)="abc"
^test2(3)="abc"
^test2(4)="abc"
^test2(5)="abc"
^test2(6)="abc"
^test2(7)="abc"
^test2(8)="abc"
^test2(9)="abc"
^test2(10)="abc"
USER>
USER>w $ZF(-3,"/opt/iris/callout.so","AddInt",2,2)
4
USER>h
```
```bash
irisowner@d6be47c31b1b:/opt/iris$ exit
user@host:~/iris-c-lang$
```