#pragma warning(disable : 4996)
#include "iris-callin.h"
#include "mycallin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int callin_execute()
{
  int	rc= 0;
  IRIS_ASTR command;

  printf("========= Calling callin_execute\n");

  sprintf((char *)command.str,"L +exec s ^execute($INCREMENT(^execute))=$ZDATETIME($H)_\"/\"_$J L -exec");
  command.len = (unsigned short)strlen((char *)command.str);
  rc = IRISEXECUTE(&command);
  RETURNIFERROR(rc)

  return 0;
}
