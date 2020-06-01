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

ZFBEGIN
   ZFENTRY("AddInt","iiP",addTwoIntegers)
   ZFENTRY("AddIntSave","iiP",addTwoIntegersAndSave)
   ZFENTRY("WaitIRISEvent","",waitIRISEvent)
ZFEND