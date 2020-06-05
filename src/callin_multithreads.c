/*
  https://docs.intersystems.com/irislatest/csp/docbook/Doc.View.cls?KEY=BXCI_callin#BXCI_callin_multithreading_signals
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "iris-callin.h"


#define ADD_ASYNC 1     // add async signal handler

#define ADD_NON_IRIS 1    // add a thread which doesn't connect to IRIS

void sigaction_handler();

int	runtest();
void *thread_main(void *);          // threads which connect to IRIS
#ifdef ADD_NON_IRIS
void *thread_noiris_main(void *);   // a thread for non IRIS
#endif 
#ifdef ADD_ASYNC
void sigaction_handler_async();     // signal handler for Asynchronous signals
#endif

volatile sig_atomic_t eflag = 0;    // flag to end the loop

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
  int targ[NUMTHREADS];
  pthread_t th2;

  int rtn;
  int	rc;
  int	numthreads = NUMTHREADS;
  int	i;
  
  printf("Starting main process. #%ld\n",pthread_self());

  // signal handler for Synchronous signals
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = sigaction_handler;
  sa.sa_flags   = SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);

  // signal handler for Asynchronous signals, such as SIGINT
#ifdef ADD_ASYNC
  struct sigaction sa_asyncsig;
  memset(&sa_asyncsig, 0, sizeof(sa_asyncsig));
  sa_asyncsig.sa_sigaction = sigaction_handler_async;
  sa_asyncsig.sa_flags = SA_SIGINFO;

  if ( sigaction(SIGINT, &sa_asyncsig, NULL) < 0 ) {
    exit(1);
  }
#endif

#ifdef ADD_NON_IRIS
  // creating a non IRIS thread
  rc = pthread_create(&th2, NULL, thread_noiris_main, (void *) NULL);
#endif

	/* 
    NOTE:  The call to IRISSETDIR is required to set the installation directory value
	  when linking using a shared library (.dll, or .so) on Windows, and some Unix platforms.
	*/
  rc=IRISSETDIR("/usr/irissys/mgr");
  printf("IRISSETDIR rc:%d\n",rc);

  for (i=0; i < numthreads; i++) {
    targ[i]=i;
    rc = pthread_create(&th[i], NULL, thread_main, (void *) &targ[i]);
    if (rc) {
        fprintf(stderr, "pthread_create() #%0d failed for %d.", i, rtn);
        exit(EXIT_FAILURE);
    }
  }
  printf("Waiting for threads to exit...\n");
 
  for (i=0; i < numthreads; i++) {
    pthread_join(th[i], NULL);
  }
#ifdef ADD_NON_IRIS
  printf("Join th2\n");
  pthread_join(th2, NULL);
#endif
  // Wait for the threads to exit
  printf("All threads have exited - done\n");
  return 0;
}

void *thread_main(void *tparam) {
  int *p = (int *)tparam;
  int termflag = IRIS_TTNONE|IRIS_TTNEVER;
  int timeout = 15;
  IRISSTR pin;
  IRISSTR pout;
  IRISSTR pusername;
  IRISSTR ppassword;
  IRISSTR pexename;
  int	rc;
  int retval=1;
  

  /*
  strcpy((char *) pin.str,"/dev/null");
  strcpy((char *) pout.str,"/dev/null");
  pin.len = strlen((char *) pin.str);
  pout.len = strlen((char *)pout.str);
  */

  strcpy((char *) pusername.str,"_SYSTEM");
  strcpy((char *) ppassword.str,"SYS");
  strcpy((char *) pexename.str,"callin_multithreads");
  pusername.len = strlen((char *) pusername.str);
  ppassword.len = strlen((char *)ppassword.str);
  pexename.len = strlen((char *) pexename.str);

  printf("Thread(%d) #%ld starting authentication in IRIS'\n",*p,pthread_self());

  /* Authenticate using username/pw user. */
  rc = IRISSECURESTART(&pusername,&ppassword,&pexename,termflag, timeout, NULL, NULL);
  if (rc != IRIS_SUCCESS) {
    printf("Thread(%d) #%ld : IRISSecureStart returned %d\n",*p,pthread_self(), rc);
    IRISEND();
    /*
    int *foo = NULL;
    if (*p==0) { *foo=1; }
    */
    return NULL;
  }

  runtest(*p);

  printf("Thread(%d) #%ld leaving IRIS'\n",*p,pthread_self());
  IRISEND();

  printf("Thread(%d) #%ld exiting\n",*p,pthread_self());

  return NULL;
}

int runtest(int p) {
  int rc;
  Callin_char_t *gloref="callinMT";
  unsigned int newId;
  char date[64];
  time_t t;
  struct tm local;
  char data[100];
  int *foo = NULL;

  printf("Thread(%d) #%ld starting test\n",p,pthread_self());

  // Uncomment a below line to tigger SIGSEGV intentionally.
  // if (p==0) { printf("Thread(%d) #%ld is firing SIGSEGV.\n",p,pthread_self()); *foo = 1; } 

  while ( !eflag ) {

    // Do some dummy work
    sleep(rand()%5+5);

    // Get new sequence value.  Equivalent of Set newId=$INCREMENT(^callinMT)
    rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
    rc = IRISPUSHINT(1); // Increment by 1
    rc = IRISGLOBALINCREMENT(0);
    rc = IRISPOPINT(&newId);

    // Set ^callinMT(newId)="threadId:"_tid_" @ "_timestamp
    t=time(NULL);
    localtime_r(&t,&local);
    strftime(date, sizeof(date), "%Y/%m/%d %a %H:%M:%S", &local);
    sprintf(data,"threadId:%ld @ ",pthread_self());
    strcat(data,date);

    int numargs=0;
    rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
    rc = IRISPUSHINT(newId); numargs++; 
    rc = IRISPUSHSTR(strlen(data),data);
    rc = IRISGLOBALSET(numargs);  
    if (rc!=IRIS_SUCCESS) { 
      return -1;
    }
  }
  printf("Thread(%d) #%ld has completed test\n",p,pthread_self());
  return 0;
}

void sigaction_handler(int signal, siginfo_t *si, void *arg)
{
  // Never a good idea to use printf here...
  printf("Caught signal #%ld\n",pthread_self());
  // Sometimes si is not set... 
  if (si!=NULL) printf("Caught signal(%d) via sigaction_handler() Thread #%ld\n", si->si_signo,pthread_self());
  //IRISEND();  // Do not call IRISEND() here. It causes another SIGSEGV.  
  pthread_exit(0);
}

#ifdef ADD_ASYNC
void sigaction_handler_async(int sig, siginfo_t *info, void *ctx) {
  // Never a good idea to use printf here...
  printf("Signal caught by #%ld\n",pthread_self());
  if (info!=NULL) printf("si_signo:%d si_code:%d si_pid:%d si_uid:%d\n", info->si_signo, info->si_code,(int)info->si_pid, (int)info->si_uid);

  // Do not exit here because doing so will leave child processes (IRIS processes) as zombie...which is subject to be cleared by CLNDMN.
  // Instead, set a flag to finish threads which is connected to IRIS via IRISSTART(). 
  eflag = 1;
}
#endif

#ifdef ADD_NON_IRIS
void *thread_noiris_main(void *tparam) {
  printf("Starting %s #%ld\n",__func__,pthread_self());
  while ( !eflag ) { sleep(1); }
  printf("Ending %s #%ld\n", __func__,pthread_self());
  return 0;
}
#endif

