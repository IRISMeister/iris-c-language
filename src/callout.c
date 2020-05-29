#define ZF_DLL  /* Required only for dynamically linked libraries. */
#include <iris-cdzf.h>  /* Required for all Callout code. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int AddTwoIntegers(int a, int b, int*outsum) {
   *outsum = a+b;   /* set value to be returned by the $ZF function call */
   return 0;   /* set the exit status code */
}

int AddTwoIntegersAndSave(int a, int b, int*outsum) {
   int	rc = 0;
   Callin_char_t *gloref="callout";
   Callin_char_t p[100];

   *outsum = a+b;   /* set value to be returned by the $ZF function call */
   rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
   //printf("IrisPushGlobal rc:%d\n",rc);

   sprintf(p, "%d", 1);
   rc = IRISPUSHSTR(strlen((const char *)p), p);
   //printf("IrisPushStr rc:%d\n",rc);

   sprintf(p, "%d", *outsum);
   rc = IRISPUSHSTR(strlen((const char *)p), p);
   //printf("IrisPushStr rc:%d\n",rc);

   rc=IRISGLOBALSET(1);  // Set ^callout(1)="abc"
      //printf("IrisGlobalSet rc:%d\n",rc);
   if (rc!=IRIS_SUCCESS) { 
      return -1;
   }

   return 0;   /* set the exit status code */
}

ZFBEGIN
   ZFENTRY("AddInt","iiP",AddTwoIntegers)
   ZFENTRY("AddIntSave","iiP",AddTwoIntegersAndSave)
ZFEND