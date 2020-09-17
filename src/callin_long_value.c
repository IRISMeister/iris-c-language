#pragma warning(disable : 4996)
#include "iris-callin.h"
#include "mycallin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __linux__
#include <sys/time.h>
#include <sys/resource.h>
#endif
extern char *shdir;
int callin_classmethod();
int callin_classmethod_call3();

#define ASCII_LONG_DATA_SIZE 1000000
char data_ascii_long[ASCII_LONG_DATA_SIZE+1];

int main(int argc, char * argv[])
{

  int repeatcount=1;
  if (argc>1) repeatcount=atoi(argv[1]);

  // need IRIS_PROGMODE to call callin_routine_geterrorinfo() without being disconnected
  int termflag = IRIS_PROGMODE|IRIS_TTNONE|IRIS_TTNEVER;
  //int termflag = IRIS_TTNONE|IRIS_TTNEVER;
  IRIS_ASTR pusername, ppassword, pexename;
  int	rc,timeout = 0;
  const char *username="_SYSTEM", *password="SYS",*exename="callin_longvalue";

  strcpy((char *) pusername.str, username);
  pusername.len = (unsigned short)strlen(pusername.str);
  strcpy((char *) ppassword.str, password);
  ppassword.len = (unsigned short)strlen(ppassword.str);
  strcpy((char *) pexename.str,exename);
  pexename.len = (unsigned short)strlen((char *) pexename.str);

  rc=IRISSETDIR(shdir);
  printf("IRISSETDIR rc:%d\n",rc);

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, termflag, timeout, NULL, NULL);
  char* sMes;
  switch (rc)
  {
    case IRIS_ACCESSDENIED:
      sMes = "Authentication has failed. Check the audit log for the real authentication error.";
      break;
    case IRIS_ALREADYCON:
      sMes = "Connection already existed. Returned if you call IrisSecureStartH from a $ZF function.";
      break;
    case IRIS_CHANGEPASSWORD:
      sMes = "Password change required. This return value is only returned if you are using InterSystems authentication.";
      break;
    case IRIS_CONBROKEN:
      sMes = "Connection was formed and then broken, and IrisEnd has not been called to clean up.";
      break;
    case IRIS_STRTOOLONG:
      sMes = "prinp or prout is too long.";
      break;
    case IRIS_SUCCESS:
      sMes = "Success.";
      break;
    case IRIS_FAILURE:
    default:
      sMes = "An unexpected error has occurred.";
  }
  printf("IRISSECURESTART Status :%s %d \n",sMes,rc);
  if (rc) { 
    IRISEND(); 
    exit(EXIT_FAILURE);
  }

#ifdef __linux__
	struct rusage r;
#endif

  for (int i=0; i<repeatcount; i++) {
      callin_classmethod();
//      callin_classmethod_call3();
#ifdef __linux__
      if (getrusage(RUSAGE_SELF, &r) != 0) {
        printf("getrusage() error.\n");
      }
      printf("maxrss=%ld\n", r.ru_maxrss);
#endif
  }

  IRISEND();

  printf("Exiting.\n");
  exit(EXIT_SUCCESS);
}


int callin_classmethod()
{
  int	rc= 0;
  char *str="my ascii string data";
  int numargs=0;
  int classreturnvalue=1;
  Callin_char_t *classname="TestClass";
  Callin_char_t *methodname="MyClassMethod4";

  printf("========= Calling callin_classmethod_call4\n");
  // Do ##class(TestClass).MyClassMethod4(str) 
  // Function spec
  //   Accept one param: string
  //   Return string(long)
  rc = IRISPUSHCLASSMETHOD(strlen(classname), classname, strlen(methodname), methodname,classreturnvalue);
  printf("IRISPUSHCLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHSTR(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISINVOKECLASSMETHOD(numargs);
  printf("IRISINVOKECLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  int type = IRISTYPE();
  printf("IRISTYPE type:");
  switch(type){
    case IRIS_ASTRING:
      printf("IRIS_ASTRING\n");
      break;
    case IRIS_WSTRING:
      printf("IRIS_WSTRING\n");
      break;
    case IRIS_LASTRING: 
      printf("IRIS_LASTRING\n");
      break;
    case IRIS_LWSTRING:
      printf("IRIS_LWSTRING\n");
      break;
    defaut:
      printf("%d\n",type);
  }

  IRIS_EXSTR longval;

  rc = IRISCONVERT(IRIS_LASTRING,&longval);
  printf("IRISCONVERT rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("longval.len:%d\n",longval.len);
  printf("longval.str.ch %p\n",longval.str.ch);

  memcpy(data_ascii_long,longval.str.ch,longval.len+1);
  data_ascii_long[longval.len] = '\0';

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("size of return value %ld\n",strlen(data_ascii_long));
  printf("return value as STRING :%.50s....\n",data_ascii_long);

  return 0;
}


int callin_classmethod_call3()
{
  int	rc= 0;
  char *str="my ascii string data";
  int numargs=0;
  int classreturnvalue=1;
  Callin_char_t *classname="TestClass";
  Callin_char_t *methodname="MyClassMethod3";

  printf("========= Calling callin_classmethod_call3\n");
  // Do ##class(TestClass).MyClassMethod3(str) 
  // Function spec
  //   Accept one param: string
  //   Return string
  rc = IRISPUSHCLASSMETHODA(strlen(classname), classname, strlen(methodname), methodname,classreturnvalue);
  printf("IRISPUSHCLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHSTRA(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISINVOKECLASSMETHOD(numargs);
  printf("IRISINVOKECLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  int type = IRISTYPE();
  printf("IRISTYPE type:");
  switch(type){
    case IRIS_ASTRING:
      printf("IRIS_ASTRING\n");
      break;
    case IRIS_WSTRING:
      printf("IRIS_WSTRING\n");
      break;
    case IRIS_LASTRING: 
      printf("IRIS_LASTRING\n");
      break;
    case IRIS_LWSTRING:
      printf("IRIS_LWSTRING\n");
      break;
    defaut:
      printf("%d\n",type);
  }

#define ASCII_DATA_SIZE 100
  unsigned char *c;
  unsigned char returnval[ASCII_DATA_SIZE+1];

  IRIS_ASTR retval;

  retval.len = 100;
  rc = IRISCONVERT(IRIS_STRING,&retval);
  printf("IRISCONVERT rc:%d\n",rc);
  RETURNIFERROR(rc)
  memcpy(returnval,retval.str,retval.len);
  returnval[retval.len] = '\0';
  printf("return value as STRING :%s\n",returnval);

  return 0;
}
