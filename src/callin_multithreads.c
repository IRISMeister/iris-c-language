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
#include "mycallin.h"

#define ADD_SYNC_HANDLER        // add sync signal handler
#define ADD_ASYNC_HANDLER       // add async signal handler
#define ADD_NON_IRIS            // add a thread which doesn't connect to IRIS

#ifdef __linux__
#define THREADID pthread_self()
void sigaction_handler();       // signal handler for synchronous signals
void sigaction_handler_async(); // signal handler for asynchronous signals
#else
#define THREADID GetCurrentThreadId()
int exception_filter(unsigned int code, struct _EXCEPTION_POINTERS *ep);
BOOL WINAPI ctrl_handler(DWORD);
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

#ifdef ADD_SYNC_HANDLER
  // signal handler for a synchronous signal such as SISEGV
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = sigaction_handler;
  sa.sa_flags   = SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
#endif

#ifdef ADD_ASYNC_HANDLER
  // signal handler for an asynchronous signal, such as SIGINT
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
  printf("Waiting a NON_IRIS thread.\n");
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
#ifdef ADD_NON_IRIS
  HANDLE	threadlist[NUMTHREADS+1];
#else
  HANDLE	threadlist[NUMTHREADS];
#endif
  HANDLE	th;
  int targ[NUMTHREADS];
  
  printf("Starting main process. #%ld\n",THREADID);

#ifdef ADD_ASYNC_HANDLER
  // control handler for an asynchronous signal, such as CTRL_C_EVENT
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
    targ[i]=i;  // parameter 
    th = CreateThread(NULL,
            FALSE,
            (LPTHREAD_START_ROUTINE)thread_main,
            &targ[i],
            FALSE,
            NULL);
     if (!th) {
        rc = GetLastError();
        printf("Failed to start thread #%d, error = %d\n",i,rc);
     } else {
        threadlist[threadcnt++] = th;
    }
  }
#ifdef ADD_NON_IRIS
  printf("Waiting a NON_IRIS thread.\n");
  threadlist[threadcnt++] = th2;
#endif
  printf("Waiting for threads to exit...\n");
  rc = WaitForMultipleObjectsEx(threadcnt,threadlist,TRUE,INFINITE,TRUE);

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
    RETURNIFERROR(rc);
    rc = IRISPUSHINT(1); // Increment by 1
    RETURNIFERROR(rc);
    rc = IRISGLOBALINCREMENT(0);
    RETURNIFERROR(rc);
    rc = IRISPOPINT(&newId);
    RETURNIFERROR(rc);

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
    RETURNIFERROR(rc);
    rc = IRISPUSHINT(newId); numargs++;
    RETURNIFERROR(rc);
    rc = IRISPUSHSTR(strlen(data),data);
    RETURNIFERROR(rc);
    rc = IRISGLOBALSET(numargs);
    RETURNIFERROR(rc);

  }
  printf("Thread(%d) #%ld has completed test\n",p,THREADID);
  return 0;
}

#ifdef __linux__
// handler for SIGSEGV
void sigaction_handler(int signal, siginfo_t *si, void *arg)
{
  printf("Caught signal #%ld\n",pthread_self());
  // Sometimes si is not set... 
  if (si!=NULL) printf("Caught signal(%d) via sigaction_handler() Thread #%ld\n", si->si_signo,pthread_self());

  // Doc implies no need to call IRISEND() myself.
  // https://docs.intersystems.com/iris20191/csp/docbook/Doc.View.cls?KEY=BXCI_callin#BXCI_callin_multithreading_signals_sync
  // Synchronous signals are generated by the application itself (for example, SIGSEGV). 
  // InterSystems IRIS establishes signal handlers for all synchronous signals, and unblocks these signals for each thread when it connects to InterSystems IRIS.
  pthread_exit(0);
}
#endif

#ifndef __linux__
// handler for ACCESS_VIOLATION
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
  // Handle signals. 
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_LOGOFF_EVENT:
    printf("Event (%x) caught by %d\n\n",fdwCtrlType,THREADID);
    eflag = 1;
    return TRUE;

  // see https://support.microsoft.com/ja-jp/help/2607828
  case CTRL_CLOSE_EVENT:  
  case CTRL_SHUTDOWN_EVENT:
  default:
    return FALSE;
  }
}

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
