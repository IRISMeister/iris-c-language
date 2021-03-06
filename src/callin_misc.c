#pragma warning(disable : 4996)
#include "iris-callin.h"
#include "mycallin.h"
#include "callin_funcs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __linux__
#include <sys/time.h>
#include <sys/resource.h>
#endif
extern char *shdir;

int main(int argc, char * argv[])
{

  // need IRIS_PROGMODE to call callin_routine_geterrorinfo() without being disconnected
  int termflag = IRIS_PROGMODE|IRIS_TTNONE|IRIS_TTNEVER;
  //int termflag = IRIS_TTNONE|IRIS_TTNEVER;
  IRIS_ASTR pusername, ppassword, pexename;
  int	rc,timeout = 0;
  const char *username="_SYSTEM", *password="SYS",*exename="callin_misc";

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

#ifdef __linux__
  if (getrusage(RUSAGE_SELF, &r) != 0) {
    printf("getrusage() error.\n");
  }
  printf("maxrss=%ld\n", r.ru_maxrss);
#endif

  callin_routine_call();
  callin_function_call1();
  callin_function_call1a();
  callin_function_call2();
  callin_routine_geterrorinfo();
  callin_classmethod_call1();
  callin_classmethod_call2();
  callin_classmethod_call3();
  callin_classmethod_call4();
  callin_globals_set_and_get();
  callin_execute();
  callin_value_unicode();
  callin_value_long_ascii();
  callin_value_long_unicode();

#ifdef __linux__
  if (getrusage(RUSAGE_SELF, &r) != 0) {
    printf("getrusage() error.\n");
  }
  printf("maxrss=%ld\n", r.ru_maxrss);
#endif

  IRISEND();

  printf("Exiting.\n");
  exit(EXIT_SUCCESS);
}


