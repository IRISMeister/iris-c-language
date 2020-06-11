#include "iris-callin.h"
#include "mycallin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

/*
*  Setting Unicode Value
*/
int callin_value_unicode() 
{
  int rc;
  int numargs=0;
  Callin_char_t *gloref="unicode";

  printf("========= Setting Unicode Value.\n");

  // just in case
  char *locale;
  locale = setlocale(LC_ALL, "ja_JP.UTF-8");
  printf("locale %s.\n",locale);

  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  rc = IRISPUSHINT(1); numargs++;
  printf("IRISPUSHINT rc:%d\n",rc);

  wchar_t data_unicode[] = L"あいうえお";
  printf("Pushing ... value:%ls length:%lu  size of one letter:%lu\n", data_unicode, wcslen(data_unicode), sizeof(data_unicode[0]));

  // linux wchar_t is 4 bytes, whilst windows' is 2 bytes....
#ifdef __linux__
  rc = IRISPUSHSTRH(wcslen((const wchar_t *)data_unicode), data_unicode);
  printf("IRISPUSHSTRH rc:%d\n",rc);
#else
  rc = IRISPUSHSTRW(wcslen((const wchar_t *)data_unicode), data_unicode);
  printf("IRISPUSHSTRW rc:%d\n", rc);
#endif

  rc=IRISGLOBALSET(numargs);
  printf("IRISGLOBALSET rc:%d\n",rc);
  if (rc) return -1;

  return 0;
}

  /*
  *  Setting long ascii String
  */
#define ASCII_LONG_DATA_SIZE 1000000
int callin_value_long_ascii()
{
  int rc;
  int numargs=0;
  Callin_char_t *gloref="long";
  IRIS_EXSTR longval;
  unsigned char *c;
  unsigned char data_ascii_long[ASCII_LONG_DATA_SIZE+1];

  printf("========= Setting long ascii String.\n");
  // partial hint https://docs.intersystems.com/irislatest/csp/docbook/DocBook.UI.Page.cls?KEY=BGCL_library#BGCL_library_linkages
  // [Using J linkages to pass strings] section

  memset(data_ascii_long, 'A', ASCII_LONG_DATA_SIZE);
  data_ascii_long[ASCII_LONG_DATA_SIZE]='\0';

  printf("strlen %ld\n",strlen(data_ascii_long));
  c=IRISEXSTRNEW(&longval,strlen(data_ascii_long));  // size up to IRIS_MAXLOSTSZ
  if (!c) {
    printf("IRISEXSTRNEWH failed.\n");
    return -1;
  }
  memcpy(c,data_ascii_long,strlen(data_ascii_long));
  longval.len=strlen(data_ascii_long);

  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHINT(1); numargs++;
  printf("IRISPUSHINT rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHEXSTR(&longval);
  printf("IRISPUSHEXSTR rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc=IRISGLOBALSET(numargs);   
  printf("IRISGLOBALSET rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);
  RETURNIFERROR(rc)

  return 0;
}

  /*
  *  Setting long Unicode String
  */
 #define UNICODE_LONG_DATA_SIZE 1000000
 int callin_value_long_unicode()
{
  int rc;
  int numargs=0;
  Callin_char_t *gloref="long";
  IRIS_EXSTR longval;
  wchar_t *c;
  //wchar_t data_long_uni[] = L"あいうえお";
  wchar_t data_long_uni[UNICODE_LONG_DATA_SIZE+1];

  printf("========= Setting long Unicode String.\n");

  wmemset(data_long_uni, L'あ', UNICODE_LONG_DATA_SIZE);
  data_long_uni[UNICODE_LONG_DATA_SIZE]=0;

  printf("wcslen %ld\n",wcslen(data_long_uni));

  // max size: IRIS_MAXLOSTSZ
#ifdef __linux__
  c=IRISEXSTRNEWH(&longval,wcslen(data_long_uni));
#else
  c=IRISEXSTRNEWW(&longval,wcslen(data_long_uni));
#endif
  if (!c) {
    printf("IRISEXSTRNEWx failed.\n");
    return -1;
  }
  wmemcpy(c,data_long_uni,wcslen(data_long_uni));
  longval.len=wcslen(data_long_uni);

  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHINT(2); numargs++;
  printf("IRISPUSHINT rc:%d\n",rc);
  RETURNIFERROR(rc)

#ifdef __linux__
  rc = IRISPUSHEXSTRH(&longval);
  printf("IRISPUSHEXSTRH rc:%d\n",rc);
#else
  rc = IRISPUSHEXSTRW(&longval);
  printf("IRISPUSHEXSTRW rc:%d\n",rc);
#endif
  RETURNIFERROR(rc)

  rc=IRISGLOBALSET(numargs);   
  printf("IRISGLOBALSET rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);
  RETURNIFERROR(rc)

  return 0;
}
