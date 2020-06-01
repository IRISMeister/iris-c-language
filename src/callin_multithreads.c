#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "iris-callin.h"

int	runtest();
void	*thread_main(void *);

#define NUMTHREADS 3	/* Number of threads */

/*
    This is a sample of how to utilize the call-in interface in a multi-threaded environment.
        It starts multiple threads, and within each thread calls IRISStart to create a connection.
        The thread then performs one action via IrisExecute().  The thread then calls IRISEnd()
        and exits.
    When all the threads have exited, the main thread exits.

*/
int main(int argc, char* argv[])
{
  int	threadcnt = 0;
  pthread_t th[NUMTHREADS];

  int rtn;
  int	rc;
  int	numthreads = NUMTHREADS;
  int	i;
  
  printf("Starting main process\n");

  rc=IRISSETDIR("/usr/irissys/mgr");
  printf("IRISSETDIR rc:%d\n",rc);

  for (i=0; i < numthreads; i++) {
    rtn = pthread_create(&th[i], NULL, thread_main, (void *) NULL);
    if (rtn != 0) {
        fprintf(stderr, "pthread_create() #%0d failed for %d.", i, rtn);
        exit(EXIT_FAILURE);
    }
  }
  printf("Waiting for threads to exit...\n");

  /* join */
  for (i=0; i < numthreads; i++) {
    pthread_join(th[i], NULL);
  }

  /* Wait for the threads to exit */
  printf("All threads have exited - done\n");
  return 0;
}

void *thread_main(void *tparam) {
  int termflag = IRIS_TTNONE|IRIS_TTNEVER;
  int timeout = 15;
  IRISSTR pin;
  IRISSTR pout;
  IRISSTR pusername;
  IRISSTR ppassword;
  IRISSTR pexename;
  int	rc;
  int retval=1;
  
  strcpy((char *) pexename.str,"samplecallint");
  pexename.len = strlen((char *) pexename.str);

  /*
  strcpy((char *) pin.str,"/dev/null");
  strcpy((char *) pout.str,"/dev/null");
  pin.len = strlen((char *) pin.str);
  pout.len = strlen((char *)pout.str);
  */

  strcpy((char *) pusername.str,"_SYSTEM");
  strcpy((char *) ppassword.str,"SYS");
  pusername.len = strlen((char *) pusername.str);
  ppassword.len = strlen((char *)ppassword.str);

  printf("Thread #%ld starting authentication in IRIS'\n",pthread_self());

  /* Authenticate using username/pw user. */
  rc = IRISSECURESTART(&pusername,&ppassword,&pexename,termflag, timeout, NULL, NULL);
  if (rc != IRIS_SUCCESS) {
    if (rc == IRIS_ACCESSDENIED) {
      printf("(thread #%ld) connect: IRISSecureStart returned Access Denied\n",pthread_self());
      pthread_exit(&retval);
    }
    if (rc == IRIS_CHANGEPASSWORD) {
      printf("(thread #%ld) connect: IRISsecureStart returned Password Change Required\n",pthread_self());
      pthread_exit(&retval);
    }
    printf("(thread #%ld) connect: IRISSecureStart returned %d\n",pthread_self(), rc);
    pthread_exit(&retval);
  }
  runtest();

  printf("Thread #%ld leaving IRIS'\n",pthread_self());
  IRISEND();

  printf("Thread #%ld exiting\n",pthread_self());

  return NULL;
}

int runtest() {
  int rc;
  Callin_char_t *gloref="callinMT";
  unsigned int newId;
  char date[64];

  printf("Thread #%ld starting test\n",pthread_self());

  // Do some dummy work
  sleep(rand()%5+1);

  //Get local timestamp
  time_t t = time(NULL);
  struct tm local;
  localtime_r(&t,&local);
  strftime(date, sizeof(date), "%Y/%m/%d %a %H:%M:%S", &local);

  // Get new sequence value.  Equivalent of Set newId=$INCREMENT(^callinMT)
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  rc = IRISPUSHINT(1); // Increment by 1
  rc = IRISGLOBALINCREMENT(0);
  rc = IRISPOPINT(&newId);

  // Set ^callinMT(newId)=timestamp
  int subsc=0;
  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
  rc = IRISPUSHINT(newId); subsc++;     /* subscript */
  rc = IRISPUSHSTR(strlen(date),date);  /* value */
  rc = IRISGLOBALSET(subsc);  
  if (rc!=IRIS_SUCCESS) { 
    return -1;
  }

  printf("Thread #%ld has completed test\n",pthread_self());
  return 0;
}

