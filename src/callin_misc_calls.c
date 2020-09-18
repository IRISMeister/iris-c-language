#pragma warning(disable : 4996)
#include "iris-callin.h"
#include "mycallin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ASCII_LONG_DATA_SIZE 1000000
#define USEMALLOC
#ifndef USEMALLOC
  char data_ascii_long[ASCII_LONG_DATA_SIZE+1];
#endif

int callin_routine_call()
{
  int	rc= 0;
  int rtnflags;
  int numargs=0;
  Callin_char_t *routinename="TestRoutine";
  Callin_char_t *entryname="Entry";

  printf("========= Calling callin_routine_call\n");
  // Do Entry^TestRoutine() 
  // Function spec
  //    No params
  //    No Return value 
  rc = IRISPUSHRTN(&rtnflags, strlen(entryname), entryname, strlen(routinename), routinename); 
  printf("IRISPUSHRTN rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISDORTN(rtnflags,numargs);
  printf("IRISDORTN rc:%d\n",rc);
  RETURNIFERROR(rc)

  return 0;
}

int callin_function_call1()
{
  int	rc= 0;
  int rtnflags;
  char *str="my ascii string data";
  int numargs=0;
  Callin_char_t *routinename="";
  Callin_char_t *entryname="";
  
  printf("========= Calling callin_function_call1\n");
  // Set tSC=Entry1^TestRoutine(str,int) 
  // Function spec
  //   Accept two params: string and integer
  //   Return integer 
  //
  routinename="TestRoutine";
  entryname="Entry1";
  numargs=0;
  rc = IRISPUSHFUNC(&rtnflags, strlen(entryname), entryname, strlen(routinename), routinename); 
  printf("IRISPUSHRTN rc:%d\n",rc);

  rc = IRISPUSHSTR(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISPUSHINT(100);
  printf("IRISPUSHINT rc:%d\n",rc);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISEXTFUN(rtnflags,numargs);
  printf("IRISEXTFUN rc:%d\n",rc);
  RETURNIFERROR(rc)

  int retval;
  rc = IRISPOPINT(&retval);
  printf("IRISPOPINT rc:%d\n",rc);
  printf("return value as INT4 :%d\n",retval);

  return 0;
}
int callin_function_call1a()
{
  int	rc= 0;
  int rtnflags;
  char *str="my ascii string data";
  int numargs=0;
  Callin_char_t *routinename="";
  Callin_char_t *entryname="";
  
  printf("========= Calling callin_function_call1a\n");
  // Set tSC=Entry1a^TestRoutine(str,int) 
  // Function spec
  //   Accept two params: string and integer
  //   Return integer 
  //
  routinename="TestRoutine";
  entryname="Entry1a";
  numargs=0;
  rc = IRISPUSHFUNC(&rtnflags, strlen(entryname), entryname, strlen(routinename), routinename); 
  printf("IRISPUSHRTN rc:%d\n",rc);

  rc = IRISPUSHSTR(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISPUSHINT(100);
  printf("IRISPUSHINT rc:%d\n",rc);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISEXTFUN(rtnflags,numargs);
  printf("IRISEXTFUN rc:%d\n",rc);
  RETURNIFERROR(rc)

  long long retval;
  rc = IRISPOPINT64(&retval);
  printf("IRISPOPINT rc:%d\n",rc);
  printf("return value as INT64 :%lld\n",retval);

  return 0;
}

int callin_function_call2()
{
  int	rc= 0;
  int rtnflags;
  char *str="my ascii string data";
  int numargs=0;
  
  printf("========= Calling callin_function_call2\n");
  // Set tSC=Entry2^TestRoutine(str) 
  // Function spec
  //   Accept one params: string
  //   Return: ascii string  
  numargs=0;
  rc = IRISPUSHFUNC(&rtnflags, strlen("Entry2"), "Entry2", strlen("TestRoutine"), "TestRoutine"); 
  printf("IRISPUSHRTN rc:%d\n",rc);

  rc = IRISPUSHSTR(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISEXTFUN(rtnflags,numargs);
  printf("IRISEXTFUN rc:%d\n",rc);
  RETURNIFERROR(rc)

  int len;
  Callin_char_t *val="";

  rc = IRISPOPSTR(&len, &val);
  printf("IRISPOPSTR rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("len:%d\n", len);
  printf("return value as STRING :%s\n",val);

  return 0;
}

// Almost identical to callin_routine_call().
// Example of how to get error info.
// See iris-callin.h for possible return code values, such as IRIS_ERSYNTAX, from each APIs.
int callin_routine_geterrorinfo()
{
  int	rc= 0;
  int rtnflags;
  int numargs=0;
  Callin_char_t *routinename="TestRoutine";
  Callin_char_t *entryname="ErrorEntry";

  printf("========= Calling callin_routine_geterrorinfo\n");
  // Do ErrorEntry^TestRoutine() 
  // Function spec
  //    No params
  //    No Return value 
  //  Always fails with <DIVIDE>ErrorEntry+1^TestRoutine error.
  rc = IRISPUSHRTN(&rtnflags, strlen(entryname), entryname, strlen(routinename), routinename); 
  printf("IRISPUSHRTN rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISDORTN(rtnflags,numargs);
  printf("IRISDORTN rc:%d\n",rc);

  // How to get error text from given return code value.
  char returnval[101];

  IRIS_ASTR retval;
  retval.len = 100; /* maximum return length of string */
  if (rc>0) {
    IRISERRXLATE(rc,&retval);
    memcpy(returnval,retval.str,retval.len);
    returnval[retval.len] = '\0';
    printf("IRISERRXLATE [%s]\n",returnval);
  }

  // How to get latest error info (equivalent of $ZE)
  IRIS_ASTR errmsg;
  IRIS_ASTR srcline;
  int offset;
  int rc2;
  errmsg.len = 50;
  srcline.len = 100;
  if (rc>0) {
    if ((rc2 = IRISERROR(&errmsg, &srcline, &offset)) != IRIS_SUCCESS) {
      printf("\r\nfailed to display error - rc = %d",rc2);
    } else {
      memcpy(returnval,errmsg.str,errmsg.len);
      returnval[errmsg.len] = '\0';
      printf("IRISERROR [%s]\n",returnval);

      memcpy(returnval,srcline.str,srcline.len);
      returnval[srcline.len] = '\0';
      printf("IRISERROR [%s]\n",returnval);

      printf("IRISERROR [%d]\n",offset);
    }
  }

  RETURNIFERROR(rc)

  return 0;
}

int callin_classmethod_call1()
{
  int	rc= 0;
  int numargs=0;
  int classreturnvalue=0;

  printf("========= Calling callin_classmethod_call1\n");
  // Do ##class(TestClass).MyClassMethod1() 
  // Function spec
  //    No params
  //    No Return value 
  rc = IRISPUSHCLASSMETHOD(strlen("TestClass"), "TestClass", strlen("MyClassMethod1"), "MyClassMethod1",classreturnvalue);
  printf("IRISPUSHCLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISINVOKECLASSMETHOD(numargs);
  printf("IRISINVOKECLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  return 0;
}

int callin_classmethod_call2()
{
  int	rc= 0;
  char *str="my ascii string data";
  int numargs=0;
  int classreturnvalue=1;
  Callin_char_t *classname="TestClass";
  Callin_char_t *methodname="MyClassMethod2";

  printf("========= Calling callin_classmethod_call2\n");
  // Do ##class(TestClass).MyClassMethod2(str,int) 
  // Function spec
  //   Accept two params: string and integer
  //   Return integer 
  rc = IRISPUSHCLASSMETHOD(strlen(classname), classname, strlen(methodname), methodname,classreturnvalue);
  printf("IRISPUSHCLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHSTR(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISPUSHINT(100);
  printf("IRISPUSHINT rc:%d\n",rc);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISINVOKECLASSMETHOD(numargs);
  printf("IRISINVOKECLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  int retval;
  rc = IRISPOPINT(&retval);
  printf("IRISPOPINT rc:%d\n",rc);
  
  printf("return value as INT4 :%d\n",retval);

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
  rc = IRISPUSHCLASSMETHOD(strlen(classname), classname, strlen(methodname), methodname,classreturnvalue);
  printf("IRISPUSHCLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)

  rc = IRISPUSHSTR(strlen(str), str);
  RETURNIFERROR(rc)
  numargs++;

  rc = IRISINVOKECLASSMETHOD(numargs);
  printf("IRISINVOKECLASSMETHOD rc:%d\n",rc);
  RETURNIFERROR(rc)
  
  int len;
  Callin_char_t *val="";

  rc = IRISPOPSTR(&len, &val);
  printf("IRISPOPSTR rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("len:%d\n", len);
  printf("return value as STRING :%s\n",val);
  return 0;
}

int callin_classmethod_call4()
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
  longval.len=0;
  longval.str.ch = NULL;

  rc = IRISPOPEXSTR(&longval);
  printf("IRISPOPEXSTR rc:%d\n",rc);
  if (rc) {
    IRISEXSTRKILL(&longval); // may fail
    return -1;
  }
  printf("len:%d\n",longval.len);
  printf("return value as STRING :%.50s....\n",longval.str.ch);

  rc=IRISEXSTRKILL(&longval); // Releases the storage associated with it.  
  printf("IRISEXSTRKILL rc:%d\n",rc);
  RETURNIFERROR(rc)

  return 0;
}

int callin_globals_set_and_get()
{
  int	rc= 0;
  int len;
  int numargs=0;
  Callin_char_t *gloref="";
  Callin_char_t *val="";
  Callin_char_t p[100];

  printf("========= Calling callin_globals_set_and_get\n");

  /* Get value */
  gloref="test";
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  numargs=0;
  rc=IRISGLOBALGET(numargs, 0);  // $Get(^test)
  printf("IRISGLOBALGET rc:%d\n",rc);
  if (rc==IRIS_ERUNDEF) { 
    printf("UNDEF\n"); 
  }

  rc = IRISPOPSTR(&len, &val);
  printf("IRISPOPSTR rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("len:%d\n", len);
  printf("value:%s\n",val);

  /* Get value */
  gloref="test";
  numargs=0;
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  printf("IRISPUSHGLOBAL rc:%d\n",rc);

  rc = IRISPUSHINT(1); numargs++;
  printf("IRISPUSHINT rc:%d\n",rc);

  rc=IRISGLOBALGET(numargs, 0);  // $Get(^test(1))
  printf("IRISGLOBALGET rc:%d\n",rc);
  if (rc==IRIS_ERUNDEF) { 
    printf("UNDEF\n"); 
  }

  rc = IRISPOPSTR(&len, &val);
  printf("IRISPOPSTR rc:%d\n",rc);
  RETURNIFERROR(rc)

  printf("len:%d\n", len);
  printf("value:%s\n",val);

  /* Set value ; Equivalent of For i=1:1:10 { Set ^test2(i)="abc"} */
  gloref="test2";
  for (int i=1;i<=10;i++) {
    rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
    //printf("IRISPUSHGLOBAL rc:%d\n",rc);

    rc = IRISPUSHINT(i);
    //printf("IRISPUSHSTR rc:%d\n",rc);

    strcpy(p,"abc");
    rc = IRISPUSHSTR(strlen((const char *)p), p);
    //printf("IRISPUSHSTR rc:%d\n",rc);

    rc=IRISGLOBALSET(1);  // Set ^test2(i)="abc"
    //printf("IRISGLOBALSET rc:%d\n",rc);
    if (rc!=IRIS_SUCCESS) { 
      goto end;
    }
  }
end:

  return 0;
}