#define ZF_DLL  /* Required only for dynamically linked libraries. */
#include <iris-cdzf.h>  /* Required for all Callout code. */

int AddTwoIntegers(int a, int b, int*outsum) {
   *outsum = a+b;   /* set value to be returned by the $ZF function call */
   return 0;   /* set the exit status code */
}
ZFBEGIN
   ZFENTRY("AddInt","iiP",AddTwoIntegers)
ZFEND