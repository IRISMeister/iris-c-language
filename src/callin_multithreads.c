/*
 *  https://docs.intersystems.com/irislatestj/csp/docbook/Doc.View.cls?KEY=BXCI_callin#BXCI_callin_multithreading_signals
 */
#pragma warning(disable : 4996)
#ifdef __linux__
#include <pthread.h>
#include <unistd.h>
#else
#include <windows.h>
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "iris-callin.h"

#define ADD_ASYNC        // add async signal handler
#define ADD_NON_IRIS     // add a thread which doesn't connect to IRIS

#ifdef __linux__
#define THREADID pthread_self()
void sigaction_handler();       // signal handler for synchronous signals
#ifdef ADD_ASYNC
void sigaction_handler_async(); // signal handler for asynchronous signals
#endif
#else
#define THREADID GetCurrentThreadId()
int exception_filter(unsigned int code, struct _EXCEPTION_POINTERS *ep);
#ifdef ADD_ASYNC
BOOL WINAPI ctrl_handler(DWORD);
#endif
#endif

void *thread_main(void *);      // threads which connect to IRIS
int	runtest();                  // main logic 

#ifdef ADD_NON_IRIS
void *thread_noiris_main(void *);   // a thread for non IRIS
#endif

volatile sig_atomic_t eflag = 0;    // flag to end the loop
#define NUMTHREADS 3	/* Number of threads */

/*
  This is a sample of how to utilize the call-in interface in a multi-threaded environment.
  It starts multiple threads, and within each thread calls IRISStart to create a connection.
  The thread then loops and keeps issuing IrisPushGlobal() until user interrupt it by Ctrl-c.  
  The thread then exits from the loop and calls IRISEnd() and exits.
  When all the threads have exited, the main thread exits.
*/
#ifdef __linux__
int main(int argc, char* argv[])
{
  int	threadcnt = 0;
  int	rc;
  int	numthreads = NUMTHREADS;
  int	i;
  pthread_t th[NUMTHREADS];
  int targ[NUMTHREADS];
  pthread_t th2;

  printf("Starting main process. #%ld\n",THREADID);

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
   * NOTE:  The call to IRISSETDIR is required to set the installation directory value
   * when linking using a shared library (.dll, or .so) on Windows, and some Unix platforms.
   */	
  rc=IRISSETDIR("/usr/irissys/mgr");
  printf("IRISSETDIR rc:%d\n",rc);

  for (i=0; i < numthreads; i++) {
    targ[i]=i;
    rc = pthread_create(&th[i], NULL, thread_main, (void *) &targ[i]);
    if (rc) {
        fprintf(stderr, "pthread_create() #%0d failed for %d.", i, rc);
        exit(EXIT_FAILURE);
    }
  }
  printf("Waiting for threads to exit...\n");
 
  for (i=0; i < numthreads; i++) {
    pthread_join(th[i], NULL);
  }
#ifdef ADD_NON_IRIS
  printf("waiting NON_IRIS thread\n");
  pthread_join(th2, NULL);
#endif
  printf("All threads have exited - done\n");
  return 0;
}
#else
int main(int argc, char* argv[])
{
  int	threadcnt = 0;
  int	rc;
  int	numthreads = NUMTHREADS;
  int	i;
  HANDLE	threadlist[NUMTHREADS];
  HANDLE	th;
  DWORD	exit_code;
  int targ[NUMTHREADS];
	
  printf("Starting main process. #%ld\n",THREADID);

#ifdef ADD_ASYNC
  // control handler for signals
  if (SetConsoleCtrlHandler(ctrl_handler, TRUE)) {
	printf("SetConsoleCtrlHandler in place.\n");
  }
#endif

#ifdef ADD_NON_IRIS
  // creating a non IRIS thread
  HANDLE	th2;
  th2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)thread_noiris_main,NULL,0,NULL);
#endif

  /*
   * NOTE:  The call to IRISSETDIR is required to set the installation directory value
   * when linking using a shared library (.dll, or .so) on Windows, and some Unix platforms.
   */
  rc=IRISSETDIR("C:\\InterSystems\\IRIS\\mgr");
  printf("IRISSETDIR rc:%d\n",rc);

  for (i=0; i < numthreads; i++) {
	targ[i]=i;
  	th = CreateThread(NULL,
			      0,
			      (LPTHREAD_START_ROUTINE)thread_main,
			      &targ[i],
			      0,
			      NULL);
	   if (!th) {
	      rc = GetLastError();
	      printf("Failed to start thread #%d, error = %d\n",i,rc);
	   } else {
	      threadlist[threadcnt++] = th;
  	}
  }
  printf("Waiting for threads to exit...\n");

  /* Wait for the threads to exit */
  while (threadcnt) {
	   rc = WaitForMultipleObjectsEx(threadcnt,threadlist,0,INFINITE,1);
	   if ((rc == WAIT_OBJECT_0) ||
	       ((rc > WAIT_OBJECT_0) && ((unsigned int)rc < (WAIT_OBJECT_0 + threadcnt)))) {
	      rc -= WAIT_OBJECT_0;
	      if (GetExitCodeThread(threadlist[rc],&exit_code)) {
		 if (exit_code != STILL_ACTIVE) {
		    if (--threadcnt) {
		       /* Fill dead thread's slot with the last thread
		          in the list and drop the last thread from
		          threadcnt */
		       threadlist[rc] = threadlist[threadcnt];
		    }
		 }
	      }
	   } else if (rc == -1) {
	   /* some kind of error */
	      rc = GetLastError();
	      printf("Unexpected error: %d\n",rc);
	   }
  }
#ifdef ADD_NON_IRIS
  printf("waiting NON_IRIS thread\n");
  WaitForSingleObject(th2, INFINITE);
#endif
  printf("All threads have exited - done\n");
  return 0;
}
#endif

void *thread_main(void *tparam) {
  int *p = (int *)tparam;
  int termflag = IRIS_TTNONE|IRIS_TTNEVER;
  int timeout = 15;
  //IRISSTR pin;
  //IRISSTR pout;
  IRISSTR pusername;
  IRISSTR ppassword;
  IRISSTR pexename;
  int rc;
  int retval=1;
  int *foo = NULL;

  /*
  strcpy((char *) pin.str,"/dev/null");  // Linux null device
  strcpy((char *) pout.str,"/dev/null"); //
  strcpy((char *) pin.str,"//./nul");    // Windows null device
  strcpy((char *) pout.str,"//./nul");   //
  pin.len = (unsigned short)strlen((char *) pin.str);
  pout.len = (unsigned short)strlen((char *)pout.str);
  */

  strcpy((char *) pusername.str,"_SYSTEM");
  strcpy((char *) ppassword.str,"SYS");
  strcpy((char *) pexename.str,"callin_multithreads");
  pusername.len = (unsigned short)strlen((char *) pusername.str);
  ppassword.len = (unsigned short)strlen((char *)ppassword.str);
  pexename.len = (unsigned short)strlen((char *) pexename.str);
  
  printf("Thread(%d) #%ld starting authentication in IRIS'\n",*p,THREADID);
	/* Authenticate using username/pw user. */

#ifndef __linux__
  // To trap hardware exceptions on Windows
  // https://docs.microsoft.com/ja-jp/cpp/cpp/hardware-exceptions?view=vs-2019
  __try {
#endif
	rc = IRISSECURESTART(&pusername,&ppassword,&pexename,termflag, timeout, NULL, NULL);
	if (rc) {
	  printf("Thread(%d) #%ld : IRISSecureStart returned %d\n",*p,THREADID, rc);
	  IRISEND();
	  return NULL;
	}

	//if (*p == 0) { *foo = 1; }  // comment out to throw SIGSEGV

	runtest(*p);
#ifndef __linux__
  }
  __except (exception_filter(GetExceptionCode(), GetExceptionInformation())) {}
#endif

  printf("Thread(%d) #%ld leaving IRIS'\n",*p,THREADID);
  IRISEND();

  printf("Thread(%d) #%ld exiting\n",*p,THREADID);

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

  printf("Thread(%d) #%ld starting test\n",p,THREADID);

  // Uncomment a below line to tigger SIGSEGV intentionally.
  // if (p==0) { printf("Thread(%d) #%ld is firing SIGSEGV.\n",p,pthread_self()); *foo = 1; } 

  while ( !eflag ) {

	// Do some dummy work
#ifdef __linux__
    sleep(rand()%5+5);
#else
	  Sleep(((rand() % 5) + 5) * 1000);
#endif

    // Get new sequence value.  Equivalent of Set newId=$INCREMENT(^callinMT)
    rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
    rc = IRISPUSHINT(1); // Increment by 1
    rc = IRISGLOBALINCREMENT(0);
    rc = IRISPOPINT(&newId);

    // Set ^callinMT(newId)="threadId:"_THREADID_" @ "_timestamp
    t=time(NULL);
#ifdef __linux__
    localtime_r(&t,&local);
#else
    localtime_s(&local,&t);
#endif
    strftime(date, sizeof(date), "%Y/%m/%d %a %H:%M:%S", &local);
    sprintf(data,"threadId:%ld @ ",THREADID);
    strcat(data,date);

	  int numargs=0;
	  rc = IRISPUSHGLOBAL(strlen((const char *)gloref), gloref);
	  rc = IRISPUSHINT(newId); numargs++;
	  rc = IRISPUSHSTR(strlen(data),data);
	  rc = IRISGLOBALSET(numargs);
	  if (rc) {
		  return -1;
	  }

  }
  printf("Thread(%d) #%ld has completed test\n",p,THREADID);
  return 0;
}

#ifdef __linux__
// handler for SIGSEGV
void sigaction_handler(int signal, siginfo_t *si, void *arg)
{
  // Never a good idea to use printf here...
  printf("Caught signal #%ld\n",pthread_self());
  // Sometimes si is not set... 
  if (si!=NULL) printf("Caught signal(%d) via sigaction_handler() Thread #%ld\n", si->si_signo,pthread_self());

  // Do not call IRISEND() here. It causes another SIGSEGV. 
  //IRISEND();  
  pthread_exit(0);
}
#endif

#ifndef __linux__
// handler for SIGSEGV
int exception_filter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
  if (code == EXCEPTION_ACCESS_VIOLATION)
  {
	printf("caught ACCESS_VIOLATION. in #%d\n",THREADID);
	return EXCEPTION_EXECUTE_HANDLER;
  }
  return EXCEPTION_CONTINUE_SEARCH;
}
#endif

#ifdef ADD_ASYNC
#ifdef __linux__
void sigaction_handler_async(int sig, siginfo_t *info, void *ctx) {
  // Never a good idea to use printf here...printf() is not async-signal-safe functions!!!
  printf("Signal caught by #%ld\n",THREADID);
  if (info!=NULL) printf("si_signo:%d si_code:%d si_pid:%d si_uid:%d\n", info->si_signo, info->si_code,(int)info->si_pid, (int)info->si_uid);
  // Do not exit here because doing so will leave child processes (IRIS processes) as zombie...which is subject to be cleared by CLNDMN.
  // Instead, set a flag to finish threads which is connected to IRIS via IRISSTART(). 
  eflag = 1;
}
#else
// https://docs.microsoft.com/en-us/windows/console/registering-a-control-handler-function
BOOL WINAPI ctrl_handler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event caught by %d\n\n",THREADID);
		eflag = 1;
		return TRUE;

	default:
		return FALSE;
	}
}

#endif
#endif

#ifdef ADD_NON_IRIS
void *thread_noiris_main(void *tparam) {
  printf("Starting %s #%ld\n",__func__,THREADID);
  while ( !eflag ) { 
#ifdef __linux__
  sleep(1); 
#else
  Sleep(1000);
#endif
  }
  printf("Ending %s #%ld\n", __func__,THREADID);
  return 0;
}
#endif
