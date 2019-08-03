#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo;

u32 timeout = 0;

void requestInitTimeOut(void)
{
  timeout = OS_GetTime();
}

bool requestHasTimeOut(int rtimeout)
{
  bool isTimeout =  ((OS_GetTime() - timeout) > rtimeout);
  if(isTimeout)
    closeRequestCommandInfo(false);
  return isTimeout;
}

void resetRequestCommandInfo(void)
{
  memset(requestCommandInfo.cmd_rev_buf,0,sizeof(requestCommandInfo.cmd_rev_buf));
  requestCommandInfo.cmd_rev_buf_pos=0;
  requestCommandInfo.inWaitResponse = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.done = false;
  requestCommandInfo.inError = false;
  requestCommandInfo.asyncCallback[0] = &async_M115_callback;
}

void closeRequestCommandInfo(bool isOK)
{
  requestCommandInfo.done = true;
  requestCommandInfo.inResponse = false;
  requestCommandInfo.inError = !isOK;
}

// Supported Autoreports
enum {
  AUTOREPORT_M27,
  AUTOREPORT_M155,

  SIZE_AUTOREPORTS
};

static int autoreportTimes[SIZE_AUTOREPORTS] = { 300, 300 };

void suspendAutoreports()
{
  #ifdef M27_AUTOREPORT
    request_M27(0);
  #endif
  #ifdef M155_AUTOREPORT
    async_M155(0);
  #endif
}

void resumeAutoreports()
{
  #ifdef M27_AUTOREPORT
    request_M27(autoreportTimes[AUTOREPORT_M27]);
  #endif
  #ifdef M155_AUTOREPORT
    async_M155(autoreportTimes[AUTOREPORT_M155]);
  #endif
}

static u32 lastCheck = 0;
void loopAutoreportRefresh(void)
{
    if  ((OS_GetTime() - lastCheck) > 3000) // 30sec
    {
        #ifdef M27_AUTOREPORT
          request_M27(autoreportTimes[AUTOREPORT_M27]);
        #endif
        #ifdef M155_AUTOREPORT
          async_M155(autoreportTimes[AUTOREPORT_M155]);
        #endif
        lastCheck = OS_GetTime();
    }
}


/*
    Send M21 command and wait for response

    >>> M21
    SENDING:M21
    echo:SD card ok
    echo:SD init fail

*/
bool request_M21(void)
{
  strcpy(requestCommandInfo.command,"M21\n");
  strcpy(requestCommandInfo.startMagic,echomagic);
  strcpy(requestCommandInfo.stopMagic,"SD card ok");
  strcpy(requestCommandInfo.errorMagic,"SD init fail");

  resetRequestCommandInfo();
  mustStoreCmd(requestCommandInfo.command);
  requestInitTimeOut();
  // Wait for response
  while (!requestCommandInfo.done && !requestHasTimeOut(300))
  {
    loopProcess();
  }
  // Check reponse
  if(!requestCommandInfo.inError){
        return true;
  } else {
        return false;            
  }
}

/*
SENDING:M20
Begin file list
PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~3.GCO 11081207
/YEST~1/TEST2/PI3MK2~2.GCO 11081207
/YEST~1/TEST2/PI3MK2~4.GCO 11081207
/YEST~1/TEST2/PI3MK2~5.GCO 11081207
/YEST~1/PI3MK2~1.GCO 11081207
/YEST~1/PI3MK2~3.GCO 11081207
/YEST~1/PI3MK2~2.GCO 11081207
End file list
*/
char **request_M20(void)
{
  strcpy(requestCommandInfo.command,"M20\n");
  strcpy(requestCommandInfo.startMagic,"Begin file list");
  strcpy(requestCommandInfo.stopMagic,"End file list");
  strcpy(requestCommandInfo.errorMagic,"Error");
  resetRequestCommandInfo();
  suspendAutoreports();
  mustStoreCmd(requestCommandInfo.command);
  requestInitTimeOut();
  // Wait for response
  while (!requestCommandInfo.done && !requestHasTimeOut(1000)) // SD card reader is very slow for large dirs!
  {
    loopProcess();
  }
  resumeAutoreports();
  return requestCommandInfo.cmd_rev_buf;
}


/**
 * Select the file to print 
 * 
 * >>> m23 YEST~1/TEST2/PI3MK2~5.GCO
 * SENDING:M23 YEST~1/TEST2/PI3MK2~5.GCO
 * echo:Now fresh file: YEST~1/TEST2/PI3MK2~5.GCO
 * File opened: PI3MK2~5.GCO Size: 11081207
 * File selected
 **/
long request_M23(char *filename)
{
  sprintf(requestCommandInfo.command, "M23 %s\n",filename);
  strcpy(requestCommandInfo.startMagic,echomagic);
  strcpy(requestCommandInfo.stopMagic,"File selected");
  strcpy(requestCommandInfo.errorMagic,"open failed");
  resetRequestCommandInfo();
  suspendAutoreports();
  mustStoreCmd(requestCommandInfo.command);
  requestInitTimeOut();
  // Wait for response
  while (!requestCommandInfo.done && !requestHasTimeOut(300))
  {
    loopProcess();
  }
  resumeAutoreports();
  // Find file size and report its.
  char *ptr;
  for(int i=0; i < requestCommandInfo.cmd_rev_buf_pos;i++)
  {
    char *pos = strstr(requestCommandInfo.cmd_rev_buf[i],"Size:");
    if(pos != NULL)
    {
      return strtol(pos+5, &ptr, 10);
    }
  }
  return -1;
}

/**
 * Start o resume print 
 **/
bool request_M24(int pos)
{
  if(pos == 0){
      mustStoreCmd("M24\n");
  } else {
      mustStoreCmd("M24 S%d\n",pos);
  }
  return true;
}

/**
 * Abort print 
 **/
bool request_M524(void)
{
  mustStoreCmd("M524\n");
  return true;
}
/**
 * Pause print 
 **/
bool request_M25(void)
{
  mustStoreCmd("M25\n");
  return true;
}

/**
 * Print status ( start auto report)
 * ->  SD printing byte 123/12345
 * ->  Not SD printing
 **/
bool request_M27(int seconds)
{
  if(seconds != 0)
     autoreportTimes[AUTOREPORT_M27]=seconds;
  mustStoreCmd("M27 S%d\n",seconds);
  return true;
}

/**
 * Temperature auto report ( same as M105 )
 **/
bool async_M155(int seconds)
{
  if(seconds != 0)
      autoreportTimes[AUTOREPORT_M155]=seconds;
  return storeCmd("M155 S%d\n",seconds);
}

/**
 *   Read capabilities from M115 Printer
 *  Es:
FIRMWARE_NAME:Marlin bugfix-2.0.x (Github) SOURCE_CODE_URL:https://github.com/MarlinFirmware/Marlin PROTOCOL_VERSION:1.0 MACHINE_TYPE:CTC/SKR i3 Pro EXTRUDER_COUNT:1 UUID:cf2e20ab-6ece-48e1-84cc-be460024c3df
Cap:SERIAL_XON_XOFF:0
Cap:BINARY_FILE_TRANSFER:0
Cap:EEPROM:1
Cap:VOLUMETRIC:1
Cap:AUTOREPORT_TEMP:1
Cap:PROGRESS:0
Cap:PRINT_JOB:1
Cap:AUTOLEVEL:1
Cap:Z_PROBE:1
Cap:LEVELING_DATA:1
Cap:BUILD_PERCENT:0
Cap:SOFTWARE_POWER:0
Cap:TOGGLE_LIGHTS:0
Cap:CASE_LIGHT_BRIGHTNESS:0
Cap:EMERGENCY_PARSER:0
Cap:PROMPT_SUPPORT:0
Cap:AUTOREPORT_SD_STATUS:1
Cap:THERMAL_PROTECTION:1
Cap:MOTION_MODES:0
Cap:CHAMBER_TEMPERATURE:0
 */
static M115_CAP cap;

M115_CAP *async_M115(void)
{
  if(OS_GetTime() - cap.lastUpdateTime > 300 ){
    resetRequestCommandInfo();
    storeCmd("M115\n");
  }
  return &cap;
}

void async_M115_callback(char *buffer)
{
    if(strstr(buffer,"EXTRUDER_COUNT:") != NULL){
      char *ptr;
      cap._EXTRUDER_COUNT = strtol(strstr(buffer,"EXTRUDER_COUNT:")+15, &ptr, 10);
      cap.lastUpdateTime = OS_GetTime();
    }
    if(strstr(buffer,"UUID:") != NULL)
    {
      strncpy(cap._UUID,strstr(buffer,"UUID:")+5,36);
      cap.lastUpdateTime = OS_GetTime();
    }
    if(strstr(buffer,"Cap:") != NULL)
    {
      cap._AUTOREPORT_TEMP |= strstr(buffer,"AUTOREPORT_TEMP:1") != NULL;
      cap._PROGRESS |= strstr(buffer,"PROGRESS:1") != NULL;
      cap._AUTOLEVEL |= strstr(buffer,"AUTOLEVEL:1") != NULL;
      cap._Z_PROBE |= strstr(buffer,"Z_PROBE:1") != NULL;
      cap._PRINT_JOB |= strstr(buffer,"PRINT_JOB:1") != NULL;
      cap._LEVELING_DATA |= strstr(buffer,"LEVELING_DATA:1") != NULL;
      cap._BUILD_PERCENT |= strstr(buffer,"BUILD_PERCENT:1") != NULL;
      cap._VOLUMETRIC |= strstr(buffer,"VOLUMETRIC:1") != NULL;
      cap._SOFTWARE_POWER |= strstr(buffer,"SOFTWARE_POWER:1") != NULL;
      cap._TOGGLE_LIGHTS |= strstr(buffer,"TOGGLE_LIGHTS:1") != NULL;
      cap._CASE_LIGHT_BRIGHTNESS |= strstr(buffer,"CASE_LIGHT_BRIGHTNESS:1") != NULL;
      cap._EMERGENCY_PARSER |= strstr(buffer,"EMERGENCY_PARSER:1") != NULL;
      cap._PROMPT_SUPPORT |= strstr(buffer,"PROMPT_SUPPORT:1") != NULL;
      cap._AUTOREPORT_SD_STATUS |= strstr(buffer,"AUTOREPORT_SD_STATUS:1") != NULL;
      cap._THERMAL_PROTECTION |= strstr(buffer,"THERMAL_PROTECTION:1") != NULL;
      cap._MOTION_MODES |= strstr(buffer,"MOTION_MODES:1") != NULL;
      cap._CHAMBER_TEMPERATURE |= strstr(buffer,"CHAMBER_TEMPERATURE:1") != NULL;
      cap.lastUpdateTime = OS_GetTime();
    }
}