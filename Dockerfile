FROM store/intersystems/iris-community:2020.1.0.215.0

USER root

# Japanese language pack 
RUN apt -y update \
 && DEBIAN_FRONTEND=noninteractive apt -y install language-pack-ja-base language-pack-ja ibus-mozc 

# installing vim just for convenience
RUN apt -y update \
 && DEBIAN_FRONTEND=noninteractive apt -y install build-essential vim \
 && apt clean

USER irisowner

RUN echo 'export LANG=ja_JP.UTF-8' >> ~/.bashrc && echo 'export LANGUAGE="ja_JP:ja"' >> ~/.bashrc \
 && mkdir src iris

COPY src/ /home/irisowner/src/
COPY iris/ /home/irisowner/iris/

RUN cd src && ./compile.sh 

RUN iris start $ISC_PACKAGE_INSTANCENAME quietly \ 
 && printf 'Do ##class(Config.NLS.Locales).Install("jpuw") Do ##class(Security.Users).UnExpireUserPasswords("*") Set p("Enabled")=1 Do ##class(Security.Services).Modify("%%Service_CallIn", .p) h\n' | iris session $ISC_PACKAGE_INSTANCENAME -U %SYS \
 && printf 'Do $SYSTEM.OBJ.ImportDir("/home/irisowner/iris",,"ck",,1) Set ^test="abc" For i=1:1:10 { Set ^test(i)=i } h\n' | iris session $ISC_PACKAGE_INSTANCENAME \
 && iris stop $ISC_PACKAGE_INSTANCENAME quietly

# Cleaning up
RUN iris start $ISC_PACKAGE_INSTANCENAME nostu quietly \
 && printf "kill ^%%SYS(\"JOURNAL\") kill ^SYS(\"NODE\") h\n" | iris session $ISC_PACKAGE_INSTANCENAME -B | cat \
 && iris stop $ISC_PACKAGE_INSTANCENAME quietly bypass \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/journal.log \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/IRIS.WIJ \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/iris.ids \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/alerts.log \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/journal/* \
 && rm -f $ISC_PACKAGE_INSTALLDIR/mgr/messages.log
