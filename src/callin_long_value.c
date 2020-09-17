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

#define ASCII_LONG_DATA_SIZE 1000000
//#define USEMALLOC
#ifndef USEMALLOC
  char data_ascii_long[ASCII_LONG_DATA_SIZE+1];
#endif


int main(int argc, char * argv[])
{

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

  for (int i=0; i<10000; i++) {
      callin_classmethod();
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

  IRIS_EXSTR longval;

  rc = IRISCONVERT(IRIS_LASTRING,&longval);
  printf("IRISCONVERT rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("longval.len:%d\n",longval.len);
  printf("longval.str.ch %p\n",longval.str.ch);

  // Allocate user data area
#ifdef USEMALLOC
  char *data_ascii_long;
  data_ascii_long = (char *)malloc(longval.len+1);
  if(data_ascii_long == NULL) {
    printf("malloc failed.\n");
    rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
    printf("IRISEXSTRKILL rc:%d\n",rc);
    return -1;
  }
#endif
  memcpy(data_ascii_long,longval.str.ch,longval.len+1);
  data_ascii_long[longval.len] = '\0';

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("size of return value %ld\n",strlen(data_ascii_long));
  printf("return value as STRING :%.50s....\n",data_ascii_long);
#ifdef USEMALLOC
  free(data_ascii_long);
#endif

  return 0;
}
