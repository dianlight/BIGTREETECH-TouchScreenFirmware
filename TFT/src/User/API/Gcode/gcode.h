#ifndef _GCODE_H_
#define _GCODE_H_
#include "stdbool.h"
#include "interfaceCmd.h"
#include "parseACK.h"

#define CMD_MAX_SIZE    100
#define CMD_ASYNC       1                      // Number of Async procedures

typedef struct {
    char command[CMD_MAX_CHAR];                // The command sent to printer
    char startMagic[CMD_MAX_CHAR];             // The magic to identify the start
    char stopMagic[CMD_MAX_CHAR];              // The magic to identify the stop
    char errorMagic[CMD_MAX_CHAR];             // The magic to identify the error response
    bool inResponse;                           // true if between start and stop magic
    bool inWaitResponse;                       // true if waiting for start magic
    bool done;                                 // true if command is executed and response is received
    bool inError;                              // true if error response
    char *cmd_rev_buf[ACK_MAX_LINE];           // buffer where store the command response (only ponter to line)
    u8 cmd_rev_buf_pos;                        // current position in cmd_rev_buf list
    void (*asyncCallback[CMD_ASYNC])(char *);  // list of function pointer for async gecode functions;
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

void resetRequestCommandInfo(void);
void closeRequestCommandInfo(bool isOK);

void loopAutoreportRefresh(void);

bool request_M21(void);
char **request_M20(void);
bool request_M25(void);
bool request_M27(int seconds);
bool request_M524(void);
bool request_M24(int pos);
long request_M23(char *filename);

bool async_M155(int seconds);

typedef struct {
    uint32_t    lastUpdateTime;
    long        _EXTRUDER_COUNT;
    char        _UUID[36];
    bool        _AUTOREPORT_TEMP;
    bool        _PROGRESS;
    bool        _AUTOLEVEL;
    bool        _Z_PROBE;
    bool        _PRINT_JOB;
    bool        _LEVELING_DATA;
    bool        _BUILD_PERCENT;
    bool        _VOLUMETRIC;
    bool        _SOFTWARE_POWER;
    bool        _TOGGLE_LIGHTS;
    bool        _CASE_LIGHT_BRIGHTNESS;
    bool        _EMERGENCY_PARSER;
    bool        _PROMPT_SUPPORT;
    bool        _AUTOREPORT_SD_STATUS;
    bool        _THERMAL_PROTECTION;
    bool        _MOTION_MODES;
    bool        _CHAMBER_TEMPERATURE;
} M115_CAP;

M115_CAP *async_M115(void);
void async_M115_callback(char *buffer);

#endif
