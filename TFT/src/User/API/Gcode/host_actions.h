#ifndef _HOST_ACTION_H_
#define _HOST_ACTION_H_

#include "includes.h"


bool parseHostAction(char *action);

void sendActionCommandPause(void);
void sendActionCommandResume(void);
void sendActionCommandCancel(void);



#endif
