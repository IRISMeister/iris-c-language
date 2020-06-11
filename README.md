The simpest example of how to build/use IRIS Callin and Callout in Linux.  
Callin (callin) will get value from ^test global and set ^test2(1) to ^test2(10) global nodes.  
Multi-thread Callin (callint) will set ^CallinTest global nodes from within each thread.  
Callout (callout.so) will add two given integers and returns it.  

see https://docs.intersystems.com/iris20191j/csp/docbook/Doc.View.cls?KEY=BXCI
# How to RUN

## Start IRIS.
```bash
user@host:~/iris-c-language$ docker-compose up -d
user@host:~/iris-c-language$ docker-compose exec iris bash
irisowner@ec21549f2063:~$
```
## Various functions and various data (Unicode, Long Ascii String, Long Unicode String) handling.
```bash
irisowner@ec21549f2063:~$ cd src
irisowner@c91e96487245:~/src$ ./callin_misc
IRISSETDIR rc:0
IRISSECURESTART Status :Success. 0
========= Calling callin_routine_call
IRISPUSHRTN rc:0
IRISDORTN rc:0
========= Calling callin_function_call1
IRISPUSHRTN rc:0
IRISPUSHINT rc:0
IRISEXTFUN rc:0
IRISCONVERT rc:0
return value as INT4 :123
========= Calling callin_function_call2
IRISPUSHRTN rc:0
IRISEXTFUN rc:0
IRISCONVERT rc:0
return value as STRING :abcdefg
========= Calling callin_routine_geterrorinfo
IRISPUSHRTN rc:0
IRISDORTN rc:18
IRISERRXLATE [<DIVIDE>]
IRISERROR [<DIVIDE>ErrorEntry+1^TestRoutine]
IRISERROR [     Set a=1/0]
IRISERROR [1]
========= Calling callin_classmethod_call1
IRISPUSHCLASSMETHOD rc:0
IRISINVOKECLASSMETHOD rc:0
========= Calling callin_classmethod_call2
IRISPUSHCLASSMETHOD rc:0
IRISPUSHINT rc:0
IRISINVOKECLASSMETHOD rc:0
IRISCONVERT rc:0
return value as INT4 :456
========= Calling callin_globals_set_and_get
IRISPUSHGLOBAL rc:0
IRISGLOBALGET rc:0
IRISPOPSTR rc:0
len:44
value:06/11/2020 12:10:04;my ascii string data;100
IRISPUSHGLOBAL rc:0
IRISPUSHSTR rc:0
IRISGLOBALGET rc:0
IRISPOPSTR rc:0
len:2
value:bb
========= Calling callin_execute
========= Setting Unicode Value.
locale ja_JP.UTF-8.
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
irisowner@ec21549f2063:~/src$
irisowner@ec21549f2063:~/src$ iris session iris -U demo
ノード: ec21549f2063 インスタンス: IRIS
```
```ObjectScript
DEMO>zw ^test
^test="06/11/2020 12:10:04;my ascii string data;100"
^test(1)=12345
DEMO>
DEMO>zw ^test2
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
DEMO>
DEMO>zw ^calltest
^calltest(1)="06/11/2020 17:38:49"
^calltest(2)="06/11/2020 17:38:49;my ascii string data;100"
^calltest(3)="06/11/2020 17:38:49;my ascii string data"
DEMO>
DEMO>zw ^execute
^execute=1
^execute(1)="06/11/2020 12:10:04/566"
DEMO>
DEMO>zw ^unicode
^unicode(1)="あいうえお"
DEMO>w $L(^long(1))
1000000
DEMO>
DEMO>w $E(^long(1),$L(^long(1))-10,*)
AAAAAAAAAAA
DEMO>
DEMO>w $L(^long(2))
1000000
DEMO>
DEMO>w $E(^long(2),$L(^long(2))-10,*)
あああああああああああ
DEMO>h
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
irisowner@ec21549f2063:~/src$ iris session iris -U demo

ノード: ec21549f2063 インスタンス: IRIS
```
```ObjectScript
DEMO>zw ^callinMT
^callinMT=4
^callinMT(1)="threadId:140432100972288 @ 2020/06/04 Thu 17:20:35"
^callinMT(2)="threadId:140432084186880 @ 2020/06/04 Thu 17:20:36"
^callinMT(3)="threadId:140432092579584 @ 2020/06/04 Thu 17:20:37"
^callinMT(4)="threadId:140432100972288 @ 2020/06/04 Thu 17:20:40"
DEMO>
```
## CallOut tests.  
"AddInt" adds given two numeric values.  
"AddIntSave" does the same and then save it to a global. (a combination of callout and callin)  
"CallRoutine" calls a routine which access local variable 'MYLOCALVAR' and return as a string value.

```ObjectScript
DEMO>w $ZF(-3,"/home/irisowner/src/callout.so","AddInt",2,3)
5
DEMO>w $ZF(-3,"/home/irisowner/src/callout.so","AddIntSave",3,8)
11
DEMO>zw ^callout
^callout(1)=11
DEMO>w $ZF(-3,"/home/irisowner/src/callout.so","CallRoutine")
UNDEF
W $ZF(-3,"home/irisowner/src/callout.so","CallRoutine")
^
<FUNCTION>
DEMO>s MYLOCALVAR="abcdefg" 
DEMO>w $ZF(-3,"/home/irisowner/src/callout.so","CallRoutine")
return value as STRING :abcdefg
DEMO>h
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

