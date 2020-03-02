FROM store/intersystems/iris-community:2020.1.0.199.0

COPY src /opt/iris/

USER root

RUN apt -y update \
 && DEBIAN_FRONTEND=noninteractive apt -y install build-essential \
 && apt clean

RUN cd /opt/iris \
 && ./compile.sh 

USER irisowner

RUN iris start $ISC_PACKAGE_INSTANCENAME quietly \ 
 && printf 'Do ##class(Config.NLS.Locales).Install("jpuw") Do ##class(Security.Users).UnExpireUserPasswords("*") Set p("Enabled")=1 Do ##class(Security.Services).Modify("%%Service_CallIn", .p) h\n' | iris session $ISC_PACKAGE_INSTANCENAME -U %SYS \
 && printf 'Set ^test="abc" For i=1:1:10 { Set ^test(i)=i } h\n' | iris session $ISC_PACKAGE_INSTANCENAME \
 && iris stop $ISC_PACKAGE_INSTANCENAME quietly
