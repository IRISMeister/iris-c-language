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

// non-threadライブラリでは、IRISとのセッション確立後は(IRISSTART～IRISEND)は、登録したユーザハンドラが呼ばれることは無い。
// IRIS側で処理される。ごみプロセスも残らない。message.logに下記が記録される。
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

  rc = IRISSECURESTART(&pusername, &ppassword, &pexename, IRIS_TTNONE|IRIS_TTNEVER , timeout, NULL, NULL);
  printf("IRISSECURESTART Status :%d \n",rc);
  if (rc) exit(1);

  sleep(5);
  *foo = 1;     /* Cause a seg fault. This will not call sigaction_handler() */

  IRISEND(); 
  //*foo = 1;   /* Cause a seg fault. This will call sigaction_handler() because iris session is already over */

  printf("Ending main()\n");
  exit(0);
}

#ifdef usesigaction
void sigaction_handler(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault via sigaction_handler().\n"); 
    exit(0);
}
#else
void signal_handler(int sig) {
    printf("Caught segfault via signal_handler()\n" );
    exit(0);
}
#endif
