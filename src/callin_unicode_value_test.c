#include "iris-callin.h"
#include "mycallin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
int main(int argc, char * argv[])
{
  IRIS_ASTR pusername, ppassword, pexename;
  IRIS_ASTR command;
  int	rc,timeout = 0;
  int len;
  Callin_char_t *gloref="unicode";
  Callin_char_t *val="";
  Callin_char_t p[100];
  const char *username="_SYSTEM", *password="SYS";

  char *locale;
  locale = setlocale(LC_ALL, "ja_JP.UTF-8");
  printf("locale %s.\n",locale);


  strcpy((char *) pusername.str, username);
  pusername.len = strlen(pusername.str);
  strcpy((char *) ppassword.str, password);
  ppassword.len = strlen(ppassword.str);  
	strcpy((char *) pexename.str,"callintest");
	pexename.len = (unsigned short)strlen((char *) pexename.str);

  rc=IRISSETDIR("/usr/irissys/mgr");
  printf("rc:%d\n",rc);

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, IRIS_PROGMODE|IRIS_TTNONE , timeout, NULL, NULL);
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
  if (rc) { 
    IrisEnd(); 
    exit(1);
  }

  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  sprintf(p, "%d", 1);
  rc = IRISPUSHSTR(strlen((const char *)p), p);
  printf("IRISPUSHSTR rc:%d\n",rc);

  wchar_t wc[] = L"あいうえお";
  printf("Pushing ... value:%ls length:%lu  size of one letter:%lu\n", wc, wcslen(wc), sizeof(wc[0]));

  // linux wchar_t is 4 bytes, whilst windows' is 2 bytes....
#ifdef __linux__
   rc = IRISPUSHSTRH(wcslen((const wchar_t *)wc), wc);
#endif
   printf("IRISPUSHSTRH rc:%d\n",rc);

  rc=IRISGLOBALSET(1);  // Set ^test2(i)="abc"
   
  printf("IRISGLOBALSET rc:%d\n",rc);
  if (rc!=IRIS_SUCCESS) { 
      return -1;
   }

  printf("Exiting.\n");
  IRISEND();
  exit(0);

}
