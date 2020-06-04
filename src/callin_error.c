#include "iris-callin.h"
#include "mycallin.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define USE_SIGNAL 1
#define ADD_ASYNC 1     // add async signal handler

#ifdef USE_SIGNAL
void signal_handler();
#else
void sigaction_handler();
#endif
#ifdef ADD_ASYNC
void sigaction_handler_async();     // signal handler for Asynchronous signals
#endif

int main(void)
{

  IRIS_ASTR pusername, ppassword, pexename;
  int	rc,timeout = 0;
  const char *username="_SYSTEM", *password="SYS";
  int *foo = NULL;

#ifdef USE_SIGNAL
  if ( signal(SIGSEGV, signal_handler) == SIG_ERR ) {
    exit(1);
  } 
#else
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigaction_handler;
    sa.sa_flags   = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
#endif

  // signal handler for Asynchronous signals, such as SIGINT
#if ADD_ASYNC
  struct sigaction sa_asyncsig;
  memset(&sa_asyncsig, 0, sizeof(sa_asyncsig));
  sa_asyncsig.sa_sigaction = sigaction_handler_async;
  sa_asyncsig.sa_flags = SA_SIGINFO;

  if ( sigaction(SIGINT, &sa_asyncsig, NULL) < 0 ) { exit(1); }
  if ( sigaction(SIGHUP, &sa_asyncsig, NULL) < 0 ) { exit(1); }
#endif

  strcpy((char *) pusername.str, username);
  pusername.len = strlen(pusername.str);
  strcpy((char *) ppassword.str, password);
  ppassword.len = strlen(ppassword.str);  
	strcpy((char *) pexename.str,"callintest");
	pexename.len = (unsigned short)strlen((char *) pexename.str);

  rc=IRISSETDIR("/usr/irissys/mgr");
  printf("IRISSETDIR rc:%d\n",rc);

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, IRIS_TTNONE|IRIS_TTNEVER , timeout, NULL, NULL);
  printf("IRISSECURESTART Status :%d \n",rc);
  if (rc) exit(1);

  printf("sleeping...\n");
  sleep(10);

  //*foo = 1; /* Cause a seg fault. This will not call sigaction_handler() */
  rc=IRISEND(); 
  printf("IRISEND Status :%d \n",rc);
  //*foo = 1; /* Cause a seg fault. This will call sigaction_handler() */

  printf("Ending main()\n");
  exit(0);
}

#ifdef USE_SIGNAL
void signal_handler(int sig) {
    printf("Caught signal via signal_handler()\n" );
    exit(0);
}
#else
void sigaction_handler(int signal, siginfo_t *si, void *arg)
{
    printf("Caught signal via sigaction_handler().\n"); 
    exit(0);
}
#endif

#if ADD_ASYNC
void sigaction_handler_async(int sig, siginfo_t *info, void *ctx) {
  printf("Caught signal via sigaction_handler_async().\n"); 
  if (info!=NULL) printf("si_signo:%d si_code:%d si_pid:%d si_uid:%d\n", info->si_signo, info->si_code,(int)info->si_pid, (int)info->si_uid);
}
#endif
