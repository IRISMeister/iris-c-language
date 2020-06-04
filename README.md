The simpest example of how to build/use IRIS Callin and Callout in Linux.  
Callin (callin) will get value from ^test global and set ^test2(1) to ^test2(10) global nodes.  
Multi-thread Callin (callint) will set ^CallinTest global nodes from within each thread.  
Callout (callout.so) will add two given integers and returns it.  
# How to RUN

## Start IRIS.
```bash
user@host:~/iris-c-language$ docker-compose up -d
user@host:~/iris-c-language$ docker-compose exec iris bash
irisowner@ec21549f2063:~$
```
## Various tests.
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
## Multithreads test.
You can't stop a program which was linked against IRIS multi-threads library via Ctrl-c.
```bash
irisowner@ec21549f2063:~/src$ ./callin_multithreads
Starting main process. #140432145268544
Starting thread_noiris_main #140432109364992
IRISSETDIR rc:0
Thread(0) #140432100972288 starting authentication in IRIS'
Thread(1) #140432092579584 starting authentication in IRIS'
Waiting for threads to exit...
Thread(2) #140432084186880 starting authentication in IRIS'
Thread(1) #140432092579584 starting test
Thread(0) #140432100972288 starting test
Thread(2) #140432084186880 starting test
^CSignal caught by #140432145268544            <= type Ctrl-c to interrupt
si_signo:2 si_code:128 si_pid:0 si_uid:0
Ending thread_noiris_main #140432109364992
Thread(2) #140432084186880 has completed test
Thread(2) #140432084186880 leaving IRIS'
Thread(2) #140432084186880 exiting
Thread(1) #140432092579584 has completed test
Thread(1) #140432092579584 leaving IRIS'
Thread(1) #140432092579584 exiting
Thread(0) #140432100972288 has completed test
Thread(0) #140432100972288 leaving IRIS'
Thread(0) #140432100972288 exiting
Join th2
All threads have exited - done
irisowner@ec21549f2063:~/src$
irisowner@ec21549f2063:~/src$ iris session iris

ノード: ec21549f2063 インスタンス: IRIS
```
```ObjectScript
USER>zw ^callinMT
^callinMT=4
^callinMT(1)="threadId:140432100972288 @ 2020/06/04 Thu 17:20:35"
^callinMT(2)="threadId:140432084186880 @ 2020/06/04 Thu 17:20:36"
^callinMT(3)="threadId:140432092579584 @ 2020/06/04 Thu 17:20:37"
^callinMT(4)="threadId:140432100972288 @ 2020/06/04 Thu 17:20:40"
USER>
```
## Various data (Unicode, Long Ascii String, Long Unicode String) tests.
```bash
irisowner@ec21549f2063:~/src$ ./callin_misc_value_test
locale ja_JP.UTF-8.
IRISSETDIR rc:0
IrisSecureStartA Status :0
========= Setting Unicode Value.
IRISPUSHGLOBAL rc:0
IRISPUSHINT rc:0
Pushing ... value:あいうえお length:5  size of one letter:4
IRISPUSHSTRH rc:0
IRISGLOBALSET rc:0
========= Setting long ascii String.
strlen 1000000
IRISPUSHGLOBAL rc:0
IRISPUSHINT rc:0
IRISPUSHEXSTR rc:0
IRISGLOBALSET rc:0
IRISEXSTRKILL rc:0
========= Setting long Unicode String.
wcslen 1000000
IRISPUSHGLOBAL rc:0
IRISPUSHINT rc:0
IRISPUSHEXSTRH rc:0
IRISGLOBALSET rc:0
IRISEXSTRKILL rc:0
Exiting.
irisowner@ec21549f2063:~/src$ iris session iris
ノード: ec21549f2063 インスタンス: IRIS
```
```ObjectScript
USER>zw ^unicode
^unicode(1)="あいうえお"
USER>w $L(^long(1))
1000000
USER>w $E(^long(1),$L(^long(1))-10,*)
AAAAAAAAAAA
USER>w $L(^long(2))
1000000
USER>w $E(^long(2),$L(^long(2))-10,*)
あああああああああああ
USER>h
```
## CallOut tests.  
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

## Stop IRIS.
```bash
user@host:~/iris-c-language$ docker-compose stop
```

If you want to run callin programs against non-container version of IRIS, you need to execute it as user 'irisowner'. 
$ sudo -u irisowner ./callin

