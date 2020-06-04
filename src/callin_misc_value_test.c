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
  int len,subsc;
  Callin_char_t *gloref="unicode";
  Callin_char_t *gloref_long="long";
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
  printf("IRISSETDIR rc:%d\n",rc);

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, IRIS_TTNONE|IRIS_TTNEVER , timeout, NULL, NULL);
  printf("IrisSecureStartA Status :%d\n",rc);
  if (rc) { 
    IRISEND(); 
    exit(1);
  }
  
  /*
  *  Setting Unicode Value
  */
  printf("========= Setting Unicode Value.\n");
  // Set Unicode Value
  subsc=0;
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  rc = IRISPUSHINT(1); subsc++;
  printf("IRISPUSHINT rc:%d\n",rc);

  wchar_t data_unicode[] = L"あいうえお";
  printf("Pushing ... value:%ls length:%lu  size of one letter:%lu\n", data_unicode, wcslen(data_unicode), sizeof(data_unicode[0]));

  // linux wchar_t is 4 bytes, whilst windows' is 2 bytes....
#ifdef __linux__
  rc = IRISPUSHSTRH(wcslen((const wchar_t *)data_unicode), data_unicode);
  printf("IRISPUSHSTRH rc:%d\n",rc);
#else
  printf("NYI\n");
  IRISEND();
  exit(1);
  //rc = IRISPUSHSTRW(wcslen((const wchar_t *)wc), wc);
#endif

  rc=IRISGLOBALSET(subsc);
  printf("IRISGLOBALSET rc:%d\n",rc);
  if (rc) return -1;

  /*
  *  Setting long ascii String
  */
  printf("========= Setting long ascii String.\n");
  // partial hint https://docs.intersystems.com/irislatest/csp/docbook/DocBook.UI.Page.cls?KEY=BGCL_library#BGCL_library_linkages
  // [Using J linkages to pass strings] section

#define ASCII_LONG_DATA_SIZE 1000000

  IRIS_EXSTR longval;
  unsigned char *c;
  //unsigned char *data="abcdefg";
  unsigned char data_ascii_long[ASCII_LONG_DATA_SIZE+1];
  memset(data_ascii_long, 'A', ASCII_LONG_DATA_SIZE);
  data_ascii_long[ASCII_LONG_DATA_SIZE]='\0';

  printf("strlen %ld\n",strlen(data_ascii_long));
  c=IRISEXSTRNEW(&longval,strlen(data_ascii_long));  // size upto IRIS_MAXLOSTSZ
  if (!c) {
    printf("IRISEXSTRNEWH failed.\n");
    IRISEND();
    exit(1);
  }
  memcpy(c,data_ascii_long,strlen(data_ascii_long));
  longval.len=strlen(data_ascii_long);

  subsc=0;
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref_long), gloref_long);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  rc = IRISPUSHINT(1); subsc++;
  printf("IRISPUSHINT rc:%d\n",rc);

  rc = IRISPUSHEXSTR(&longval);
  printf("IRISPUSHEXSTR rc:%d\n",rc);

  rc=IRISGLOBALSET(subsc);   
  printf("IRISGLOBALSET rc:%d\n",rc);
  if (rc!=IRIS_SUCCESS) { 
      return -1;
   }

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);

  /*
  *  Setting long Unicode String
  */
  printf("========= Setting long Unicode String.\n");
  #define UNICODE_LONG_DATA_SIZE 1000000
  wchar_t *c2;

  //wchar_t data_long_uni[] = L"あいうえお";
  wchar_t data_long_uni[UNICODE_LONG_DATA_SIZE+1];
  wmemset(data_long_uni, L'あ', UNICODE_LONG_DATA_SIZE);
  data_long_uni[UNICODE_LONG_DATA_SIZE]=0;

  printf("wcslen %ld\n",wcslen(data_long_uni));
#ifdef __linux__
  c2=IRISEXSTRNEWH(&longval,wcslen(data_long_uni));  // max size: IRIS_MAXLOSTSZ
#else
  printf("NYI\n");
  IRISEND();
  exit(1);
#endif
  if (!c2) {
    printf("IRISEXSTRNEWH failed.\n");
    IRISEND();
    exit(1);
  }
  wmemcpy(c2,data_long_uni,wcslen(data_long_uni));
  longval.len=wcslen(data_long_uni);

  subsc=0;
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref_long), gloref_long);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  rc = IRISPUSHINT(2); subsc++;
  printf("IRISPUSHINT rc:%d\n",rc);

#ifdef __linux__
  rc = IRISPUSHEXSTRH(&longval);
  printf("IRISPUSHEXSTRH rc:%d\n",rc);
#else
  printf("NYI\n");
  IRISEND();
  exit(1);
#endif

  rc=IRISGLOBALSET(subsc);   
  printf("IRISGLOBALSET rc:%d\n",rc);
  if (rc!=IRIS_SUCCESS) { 
      return -1;
   }

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);

  printf("Exiting.\n");
  IRISEND();
  exit(0);

}
