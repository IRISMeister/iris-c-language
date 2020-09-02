#define ZF_DLL  /* Required only for dynamically linked libraries. */
#include <cdzf.h>  /* Required for all Callout code. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int addTwoIntegers(int a, int b, int*outsum) {
   *outsum = a+b;   /* set value to be returned by the $ZF function call */
   return 0;   /* set the exit status code */
}

int addTwoIntegersAndSave(int a, int b, int*outsum) {
   int	rc = 0;
   Callin_char_t *gloref="callout";
   Callin_char_t p[100];

   *outsum = a+b;   /* set value to be returned by the $ZF function call */
   rc = CACHEPUSHGLOBAL(strlen((const char *)gloref), gloref);
   rc = CACHEPUSHINT(1);
   rc = CACHEPUSHINT(*outsum);
   rc=CACHEGLOBALSET(1);  // Set ^callout(1)=outsum
   if (rc!=CACHE_SUCCESS) { 
      return -1;
   }

   return 0;   /* set the exit status code */
}
int waitIRISEvent() {
   int rc;
   CACHE_ASTR command;

   sprintf((char *)command.str,"L +wait s ^wait($INCREMENT(^wait))=$ZDATETIME($H)_\"/\"_$J L -wait");
   command.len = (unsigned short)strlen((char *)command.str);
   rc = CACHEEXECUTE(&command);
   if (rc!=CACHE_SUCCESS) { 
      return -1;
   }

   return 0;   /* set the exit status code */
}

/*
* Example to access IRIS local variable(s) from within CallOut
* Almost identical to callin_function_call2().
*/
int callRoutine() {
   int   rc= 0;
   int   rtnflags;
   int   numargs=0;

   // Set val=Entry4^TestRoutine() 
   // Function spec
  //    No params
   //   Return: ascii string  
   numargs=0;
   rc = CACHEPUSHFUNC(&rtnflags, strlen("Entry4"), "Entry4", strlen("TestRoutine"), "TestRoutine"); 
   if (rc!=CACHE_SUCCESS) { return -1; }

   rc = CACHEEXTFUN(rtnflags,numargs);
   if (rc==CACHE_ERUNDEF) { printf("UNDEF\n"); }
   if (rc!=CACHE_SUCCESS) { return -1; }

   char returnval[101];
   CACHE_ASTR retval;

   retval.len = 100;
   rc = CACHECONVERT(CACHE_STRING,&retval);
   if (rc!=CACHE_SUCCESS) { return -1; }

   memcpy(returnval,retval.str,retval.len);
   returnval[retval.len] = '\0';

   // not a good idea to printf() here...
   printf("return value as STRING :%s\n",returnval);

   return 0;   /* set the exit status code */
}

ZFBEGIN
   ZFENTRY("AddInt","iiP",addTwoIntegers)
   ZFENTRY("AddIntSave","iiP",addTwoIntegersAndSave)
   ZFENTRY("WaitIRISEvent","",waitIRISEvent)
   ZFENTRY("CallRoutine","",callRoutine)
ZFEND