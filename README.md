The simpest example of how to build/use IRIS Callin and Callout in Linux.  
Callin (callin) will get value from ^test global and set ^test2(1) to ^test2(10) global nodes.  
Multi-thread Callin (callint) will set ^CallinTest global nodes from within each thread.  
Callout (callout.so) will add two given integers and returns it.  
# How to RUN

```bash
user@host:~/iris-c-lang$ docker-compose up -d
user@host:~/iris-c-lang$ docker-compose exec iris bash
irisowner@d6be47c31b1b:~$ cd /opt/iris/
irisowner@d6be47c31b1b:/opt/iris$ ./callin
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
IrisExecuteA rc:0
Exiting.
irisowner@d6be47c31b1b:/opt/iris$
irisowner@d6be47c31b1b:/opt/iris$ ./callint
Starting main process
rc:0
Thread #139821072086784 starting authentication in IRIS'
Waiting for threads to exit...
Thread #139821055301376 starting authentication in IRIS'
Thread #139821063694080 starting authentication in IRIS'
Thread #139821055301376 starting test
Thread #139821063694080 starting test
Thread #139821072086784 starting test
Thread #139821055301376 has completed test
Thread #139821055301376 leaving IRIS'
Thread #139821055301376 exiting
Thread #139821063694080 has completed test
Thread #139821063694080 leaving IRIS'
Thread #139821063694080 exiting
Thread #139821072086784 has completed test
Thread #139821072086784 leaving IRIS'
Thread #139821072086784 exiting
All threads have exited - done
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
USER>zw ^CallinTest
^CallinTest=3
^CallinTest(1)="65440,56101/563.00023"
^CallinTest(2)="65440,56101/563.00020"
^CallinTest(3)="65440,56101/563.00024"
USER>
USER>w $ZF(-3,"/opt/iris/callout.so","AddInt",2,2)
4
USER>h
```
```bash
irisowner@d6be47c31b1b:/opt/iris$ exit
user@host:~/iris-c-lang$
```