FROM containers.intersystems.com/intersystems/iris-community:2024.1

USER root

# Japanese language pack 
RUN apt -y update \
 && DEBIAN_FRONTEND=noninteractive apt -y install language-pack-ja-base language-pack-ja ibus-mozc 

# installing vim just for convenience
RUN apt -y update \
 && DEBIAN_FRONTEND=noninteractive apt -y install build-essential gdb vim \
 && apt clean

USER irisowner

RUN echo 'export LANG=ja_JP.UTF-8' >> ~/.bashrc && echo 'export LANGUAGE="ja_JP:ja"' >> ~/.bashrc \
 && mkdir src iris

COPY --chown=irisowner:irisowner src/ /home/irisowner/src/
COPY --chown=irisowner:irisowner project/ /home/irisowner/iris/

RUN cd src && make clean && make

RUN iris start $ISC_PACKAGE_INSTANCENAME quietly \ 
 && printf 'Do ##class(Config.NLS.Locales).Install("jpuw") h\n' | iris session $ISC_PACKAGE_INSTANCENAME -U %SYS \
 && printf 'Set tSC=$system.OBJ.Load("'$HOME'/iris/MyInstallerPackage/Installer.cls","ck") Do:+tSC=0 $SYSTEM.Process.Terminate($JOB,1) h\n' | iris session $ISC_PACKAGE_INSTANCENAME \
 && printf 'Set tSC=##class(MyInstallerPackage.Installer).setup() Do:+tSC=0 $SYSTEM.Process.Terminate($JOB,1) h\n' | iris session $ISC_PACKAGE_INSTANCENAME \
 && iris stop $ISC_PACKAGE_INSTANCENAME quietly

# Cleaning up
RUN iris start $ISC_PACKAGE_INSTANCENAME nostu quietly \
 && printf "kill ^%%SYS(\"JOURNAL\") kill ^SYS(\"NODE\") h\n" | iris session $ISC_PACKAGE_INSTANCENAME -B | cat \
 && iris stop $ISC_PACKAGE_INSTANCENAME quietly bypass \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/journal.log \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/IRIS.WIJ \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/iris.ids \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/alerts.log \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/journal/* 
# && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/messages.log
