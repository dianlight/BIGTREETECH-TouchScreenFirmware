#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#define ACK_MAX_SIZE 256        // Need to be Power of 2
#define ACK_MAX_LINE 32         // Need to be Power of 2

#include "includes.h"

static const char connectmagic[]      = "FIRMWARE_NAME";
static const char errormagic[]        = "Error:";
static const char echomagic[]         = "echo:";
static const char busymagic[]         = "busy:";
#ifdef ONBOARD_SD_SUPPORT 
static const char bsdprintingmagic[]   = "SD printing byte";
static const char bsdnoprintingmagic[] = "Not SD printing";
#endif
#ifdef M118_ACTION_COMMAND
static const char commentmagic[]       = "//";
static const char actioncommandmagic[] = "action:";  // Marlin
#endif



void parseACKml(void);

#endif

