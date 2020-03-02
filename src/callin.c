#include "iris-callin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char * argv[])
{
  IRIS_ASTR pusername, ppassword, pexename;
  IRIS_ASTR command;
  int	rc,timeout = 0;
  int len;
  Callin_char_t *gloref="";
  Callin_char_t *val="";
  Callin_char_t p[100];
  const char *username="_SYSTEM", *password="SYS";

  strcpy((char *) pusername.str, username);
  pusername.len = strlen(pusername.str);
  strcpy((char *) ppassword.str, password);
  ppassword.len = strlen(ppassword.str);  
	strcpy((char *) pexename.str,"callintest");
	pexename.len = (unsigned short)strlen((char *) pexename.str);

  rc=IrisSetDir("/usr/irissys/mgr");
  printf("rc:%d\n",rc);

  rc = IrisSecureStartA(&pusername, &ppassword, &pexename, IRIS_PROGMODE|IRIS_TTNONE , timeout, NULL, NULL);
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
  printf("IrisSecureStartA Status :%s\n",sMes);

  /* Get value */
  gloref="test";
  rc = IrisPushGlobal(strlen((const char *)gloref), gloref);
  printf("IrisPushGlobal rc:%d\n",rc);

  rc=IrisGlobalGet(0, 0);  // $Get(^test)
  printf("IrisGlobalGet rc:%d\n",rc);
  if (rc==IRIS_ERUNDEF) { 
    printf("UNDEF\n"); 
  }

  rc = IrisPopStr(&len, &val);
  printf("IrisPopStr rc:%d\n",rc);
  if (rc==IRIS_SUCCESS) {
    printf("value:%s\n",val);
  }

  /* Get value */
  gloref="test";
  rc = IrisPushGlobal(strlen((const char *)gloref), gloref);
  printf("IrisPushGlobal rc:%d\n",rc);

  strcpy(p,"2");
  rc = IrisPushStr(strlen((const char *)p), p);
  printf("IrisPushStr rc:%d\n",rc);

  rc=IrisGlobalGet(1, 0);  // $Get(^test(2))
  printf("IrisGlobalGet rc:%d\n",rc);
  if (rc==IRIS_ERUNDEF) { 
    printf("UNDEF\n"); 
  }

  rc = IrisPopStr(&len, &val);
  printf("IrisPopStr rc:%d\n",rc);
  if (rc==IRIS_SUCCESS) {
    printf("value:%s\n",val);
  }

  /* Set value ; Equivalent of For i=1:1:10 { Set ^test2(i)="abc"} */
  gloref="test2";
  for (int i=1;i<=10;i++) {
    rc = IrisPushGlobal(strlen((const char *)gloref), gloref);
    //printf("IrisPushGlobal rc:%d\n",rc);

    sprintf(p, "%d", i);
    rc = IrisPushStr(strlen((const char *)p), p);
    //printf("IrisPushStr rc:%d\n",rc);

    strcpy(p,"abc");
    rc = IrisPushStr(strlen((const char *)p), p);
    //printf("IrisPushStr rc:%d\n",rc);

    rc=IrisGlobalSet(1);  // Set ^test2(i)="abc"
    //printf("IrisGlobalSet rc:%d\n",rc);
    if (rc!=IRIS_SUCCESS) { 
      goto end;
    }
  }
end:

  /* execute anonymous commands */
  sprintf(command.str,"s ^a=1"); 
  command.len = strlen(command.str);
  rc = IrisExecuteA(&command);
  printf("IrisExecuteA rc:%d\n",rc);

  IrisEnd();
  printf("Exiting.\n");
}


