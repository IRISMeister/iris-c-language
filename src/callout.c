#define ZF_DLL  /* Required only for dynamically linked libraries. */
#include <iris-cdzf.h>  /* Required for all Callout code. */
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
   rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
   rc = IRISPUSHINT(1);
   rc = IRISPUSHINT(*outsum);
   rc=IRISGLOBALSET(1);  // Set ^callout(1)=outsum
   if (rc!=IRIS_SUCCESS) { 
      return -1;
   }

   return 0;   /* set the exit status code */
}
int waitIRISEvent() {
   int rc;
   IRIS_ASTR command;

   sprintf((char *)command.str,"L +wait s ^wait($INCREMENT(^wait))=$ZDATETIME($H)_\"/\"_$J L -wait");
   command.len = (unsigned short)strlen((char *)command.str);
   rc = IRISEXECUTE(&command);
   if (rc!=IRIS_SUCCESS) { 
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
   rc = IRISPUSHFUNC(&rtnflags, strlen("Entry4"), "Entry4", strlen("TestRoutine"), "TestRoutine"); 
   if (rc!=IRIS_SUCCESS) { return -1; }

   rc = IRISEXTFUN(rtnflags,numargs);
   if (rc==IRIS_ERUNDEF) { printf("UNDEF\n"); }
   if (rc!=IRIS_SUCCESS) { return -1; }

   char returnval[101];
   IRIS_ASTR retval;

   retval.len = 100;
   rc = IRISCONVERT(IRIS_STRING,&retval);
   if (rc!=IRIS_SUCCESS) { return -1; }

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