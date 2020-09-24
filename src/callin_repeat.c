/*
 *  This is a test code to repeat calling the same function a specified number of times.
 */
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

  int repeatcount=1;
  if (argc>1) repeatcount=atoi(argv[1]);

  // need IRIS_PROGMODE to call callin_routine_geterrorinfo() without being disconnected
  int termflag = IRIS_PROGMODE|IRIS_TTNONE|IRIS_TTNEVER;
  //int termflag = IRIS_TTNONE|IRIS_TTNEVER;
  IRIS_ASTR pusername, ppassword, pexename;
  int	rc,timeout = 0;
  const char *username="_SYSTEM", *password="SYS",*exename="callin_repeat";

  strcpy((char *) pusername.str, username);
  pusername.len = (unsigned short)strlen(pusername.str);
  strcpy((char *) ppassword.str, password);
  ppassword.len = (unsigned short)strlen(ppassword.str);
  strcpy((char *) pexename.str,exename);
  pexename.len = (unsigned short)strlen((char *) pexename.str);

  rc=IRISSETDIR(shdir);
  printf("IRISSETDIR rc:%d\n",rc);

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, termflag, timeout, NULL, NULL);
  printf("IRISSECURESTART rc:%d\n",rc);
  if (rc) { 
    IRISEND(); 
    exit(EXIT_FAILURE);
  }
  //printf("hit any key to start.\n");
  //getchar();  

#ifdef __linux__
	struct rusage r;
#endif

  for (int i=0; i<repeatcount; i++) {
      callin_classmethod_call4();
//      callin_classmethod_call3();
#ifdef __linux__
      if (getrusage(RUSAGE_SELF, &r) != 0) {
        printf("getrusage() error.\n");
      }
      printf("maxrss=%ld\n", r.ru_maxrss);
#endif
  }

  //printf("hit any key to end.\n");
  //getchar();  

  IRISEND();

  printf("Exiting.\n");
  exit(EXIT_SUCCESS);
}
