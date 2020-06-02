#include "iris-callin.h"
#include "mycallin.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define usesigaction 1

#ifdef usesigaction
void sigaction_handler();
#else
void signal_handler();
#endif

// non-threadは、関係ないのか、登録したユーザハンドラが呼ばれることはなかった。
// ごみプロセスも残らなかった。IRIS側で処理した模様。
// 06/01/20-17:39:40:377 (117592) 3 [Generic.Event] Process 117592 (JobType=Callin Connection,Dumpstyle=0,Directory='/usr/irissys/mgr/user/') caught signal 11.
// 06/01/20-17:39:40:377 (117592) 3 [Generic.Event] Parent process will clean up and halt
// 06/01/20-17:39:40:377 (117592) 3 [Generic.Event] If core dumps are enabled, a core file will be created by process 117593 in the location specified by the system configuration.

int main(void)
{

  IRIS_ASTR pusername, ppassword, pexename;
  int	rc,timeout = 0;
  const char *username="_SYSTEM", *password="SYS";

    int *foo = NULL;

#ifdef usesigaction
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigaction_handler;
    sa.sa_flags   = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
#else
  if ( signal(SIGSEGV, signal_handler) == SIG_ERR ) {
    exit(1);
  } 
#endif

  strcpy((char *) pusername.str, username);
  pusername.len = strlen(pusername.str);
  strcpy((char *) ppassword.str, password);
  ppassword.len = strlen(ppassword.str);  
	strcpy((char *) pexename.str,"callintest");
	pexename.len = (unsigned short)strlen((char *) pexename.str);

  rc=IRISSETDIR("/usr/irissys/mgr");
  printf("IRISSETDIR rc:%d\n",rc);
  
  /* 
  https://docs.intersystems.com/irislatest/csp/docbook/Doc.View.cls?KEY=BXCI_callin#BXCI_callin_tips_general
  Doc says, 
  Avoid signal handling when using IrisStart()
  IrisStart sets handlers for various signals, which may conflict with signal handlers set by the calling application.
  Is this applies to IRISSECURESTART() as well?
  */

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, IRIS_TTNONE|IRIS_TTNEVER , timeout, NULL, NULL);
  char* sMes;
  switch (rc)
  {
    case IRIS_ACCESSDENIED:
      sMes = "Authentication has failed. Check the audit log for the real authentication error.";
      break;
    case IRIS_ALREADYCON:
      sMes = "Connection already existed. Returned if you call IrisSecureStartH from a $ZF function.";
      break;
    case IRIS_CHANGEPASSWORD:
      sMes = "Password change required. This return value is only returned if you are using InterSystems authentication.";
      break;
    case IRIS_CONBROKEN:
      sMes = "Connection was formed and then broken, and IrisEnd has not been called to clean up.";
      break;
    case IRIS_STRTOOLONG:
      sMes = "prinp or prout is too long.";
      break;
    case IRIS_SUCCESS:
      sMes = "Success.";
      break;
    case IRIS_FAILURE:
    default:
      sMes = "An unexpected error has occurred.";
  }
  printf("IRISSECURESTART Status :%s %d \n",sMes,rc);
  if (rc) { 
    IRISEND(); 
    exit(1);
  }

    sleep(10);
    *foo = 1;    /* Cause a seg fault */

    IRISEND(); 
    return 0;
}

#ifdef usesigaction
void sigaction_handler(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault via sigaction() at address %p\n", si->si_addr);
    exit(0);
}
#else
void signal_handler(int sig) {
    printf("Caught segfault via signal()\n" );
    exit(0);
}
#endif
