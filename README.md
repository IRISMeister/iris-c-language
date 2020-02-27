How to RUN

```bash
user@host:~/iris-c-lang$ docker-compose up -d
Creating network "iris-callin_default" with the default driver
Creating iris-callin_iris_1 ... done
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
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
IrisPushGlobal rc:0
IrisPushStr rc:0
IrisPushStr rc:0
IrisGlobalSet rc:0
Exiting.
irisowner@d6be47c31b1b:/opt/iris$
irisowner@d6be47c31b1b:/opt/iris$ iris session iris

ノード: d6be47c31b1b インスタンス: IRIS
```
```ObjectScript
USER>w $ZF(-3,"/opt/iris/callout.so","AddInt",2,2)
4
USER>h
```
```bash
irisowner@d6be47c31b1b:/opt/iris$ exit
user@host:~/iris-c-lang$
```