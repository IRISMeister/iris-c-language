The simpest example of how to build/use IRIS Callin and Callout in Linux.  
Callin (callin) will get value from ^test global and set ^test2(1) to ^test2(10) global nodes.  
Multi-thread Callin (callint) will set ^CallinTest global nodes from within each thread.  
Callout (callout.so) will add two given integers and returns it.  
# How to RUN

Start IRIS.
```bash
user@host:~/iris-c-language$ docker-compose up -d
user@host:~/iris-c-language$ docker-compose exec iris bash
irisowner@ec21549f2063:~$
```
Various tests.
```bash
irisowner@ec21549f2063:~$ cd src
irisowner@ec21549f2063:~/src$ ./callin_misc
rc:0
IrisSecureStartA Status :Success. 0
=== Calling callin_routine_call
IRISPUSHRTN rc:0
IRISDORTN rc:17
=== Calling callin_function_call1
IRISPUSHRTN rc:0
IRISPUSHINT rc:0
IRISEXTFUN rc:17
=== Calling callin_function_call2
IRISPUSHRTN rc:0
IRISEXTFUN rc:17
=== Calling callin_classmethod_call1
IRISPUSHCLASSMETHOD rc:0
IRISINVOKECLASSMETHOD rc:122
=== Calling callin_globals_set_and_get
IrisPushGlobal rc:0
IrisGlobalGet rc:0
IrisPopStr rc:0
value:abc
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisGlobalGet rc:0
IrisPopStr rc:0
value:2
Exiting.
irisowner@ec21549f2063:~/src$
irisowner@ec21549f2063:~/src$ iris session iris
ノード: ec21549f2063 インスタンス: IRIS
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
USER>h
```
Multithreads test.
```bash
irisowner@ec21549f2063:~/src$ ./callin_multithreads
Starting main process
rc:0
Thread #140380113082112 starting authentication in IRIS'
Waiting for threads to exit...
Thread #140380096296704 starting authentication in IRIS'
Thread #140380104689408 starting authentication in IRIS'
Thread #140380113082112 starting test
Thread #140380096296704 starting test
Thread #140380104689408 starting test
Thread #140380096296704 has completed test
Thread #140380096296704 leaving IRIS'
Thread #140380096296704 exiting
Thread #140380113082112 has completed test
Thread #140380113082112 leaving IRIS'
Thread #140380113082112 exiting
Thread #140380104689408 has completed test
Thread #140380104689408 leaving IRIS'
Thread #140380104689408 exiting
All threads have exited - done
irisowner@ec21549f2063:~/src$
irisowner@ec21549f2063:~/src$ iris session iris

ノード: ec21549f2063 インスタンス: IRIS
```
```ObjectScript
USER>zw ^CallinTest
^CallinTest=3
^CallinTest(1)="65527,70243/628.00013"
^CallinTest(2)="65527,70243/628.00020"
^CallinTest(3)="65527,70243/628.00023"
USER>
```
Unicode tests.
```bash
irisowner@ec21549f2063:~/src$ ./callin_unicode_value_test
locale ja_JP.UTF-8.
rc:0
IrisSecureStartA Status :Success.
IrisPushGlobal rc:0
IrisPushStr rc:0
Pushing ... value:あいうえお length:5  size of one letter:4
IrisPushStr rc:0
IrisGlobalSet rc:0
Exiting.
irisowner@ec21549f2063:~/src$ iris session iris
ノード: ec21549f2063 インスタンス: IRIS
```
```ObjectScript
USER>zw ^unicode
^unicode(1)="あいうえお"
USER>h
```
CallOut tests.  
"AddInt" add given two numeric values.  
"AddIntSave" does the same and then save it as a global. (a combination of callout and callin)
```ObjectScript
USER>w $ZF(-3,"/home/irisowner/src/callout.so","AddInt",2,3)
5
USER>w $ZF(-3,"/home/irisowner/src/callout.so","AddIntSave",3,8)
11
USER>zw ^callout
^callout(1)=11
USER>h
```
```bash
irisowner@ec21549f2063:~/src$ exit
exit
user@host:~/iris-c-language$
```

Stop IRIS.
```bash
user@host:~/iris-c-language$ docker-compose stop
```


If you want to run callin programs against non-container version of IRIS, you need to execute it as user 'irisowner'. 
$ sudo -u irisowner ./callin

