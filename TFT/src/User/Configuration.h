#ifndef _CONFIGRATION_H_
#define _CONFIGRATION_H_

/**
 * AUTOCONFIGURATION
 * NOTE: If MARLIN2_AUTOCONFIG is defined all config in this file is use only as default and the setting ( if exists or are derivable )
 * is based on referenced marlin config 
 */
#define MARLIN2_AUTOCONFIG        // if enabled the configuration is derived from Configuration.h and Configuration_adv.h 
#ifdef MARLIN2_AUTOCONFIG
  // The directory where marlin source are (git clone root)
  #define MARLIN2_HOME  /Users/ltarantino/Documents/3D Printer/Marlin Git/Merlin2.0
#endif

#define TOOL_NUM     1    //set in 1~6
#define EXTRUDER_NUM 1    //set in 1~6
#define FAN_NUM      1    //set in 1~6

#define HEAT_MAX_TEMP    {150,    275,       275,       275,       275,       275,       275}    //max temperature can be set
#define HEAT_SIGN_ID     {"B:",   "T0:",     "T1:",     "T2:",     "T3:",     "T4:",     "T5:"}
#define HEAT_DISPLAY_ID  {"Bed",  "T0",      "T1",      "T2",      "T3",      "T4",      "T5"}
#define HEAT_CMD         {"M140", "M104 T0", "M104 T1", "M104 T2", "M104 T3", "M104 T4", "M104 T5" };
#define HEAT_WAIT_CMD    {"M190", "M109 T0", "M109 T1", "M109 T2", "M109 T3", "M109 T4", "M109 T5" };

#define TOOL_CHANGE      {"T0",   "T1",      "T2",      "T3",      "T4",      "T5"}
#define EXTRUDER_ID      {"E0",   "E1",      "E2",      "E3",      "E4",      "E5"}

#define FAN_MAX_PWM      {255,       255,       255,       255,       255,       255}
#define FAN_ID           {"Fan0",    "Fan1",    "Fan2",    "Fan3",    "Fan4",    "Fan5"}
#define FAN_CMD          {"M106 P0", "M106 P1", "M106 P2", "M106 P3", "M106 P4", "M106 P5" };

#define DEFAULT_SPEED_MOVE      3000  //Move default speed  mm/min

#define EXTRUDE_SLOW_SPEED      60   //Extrude speed  mm/min
#define EXTRUDE_NORMAL_SPEED    600  //
#define EXTRUDE_FAST_SPEED      1200 //

//size of machine
#define X_MAX_POS 300
#define Y_MAX_POS 300
#define Z_MAX_POS 400

/**
 * 12864 Mode Background & Font Color Options
 * Current color options from lcd.h: BLACK, BLUE, BROWN, BRRED, CYAN, GBLUE, GRAY, GREEN, MAGENTA, RED, WHITE, YELLOW
 */
#define ST7920_BKCOLOR BLACK
#define ST7920_FNCOLOR WHITE

// Text displayed at the top of the LCD in 12864 mode
#define ST7920_BANNER_TEXT "LCD12864 Simulator"

// Ability to print gcode from Board SD via Gcode functions.
#define ONBOARD_SD_SUPPORT
#ifdef ONBOARD_SD_SUPPORT
  #define M27_AUTOREPORT                      // Disable the M27 polling if you enable enable Marlin AUTO_REPORT_SD_STATUS
  #define M27_REFRESH                 3       // Time in sec for M27 command 
  #define M27_WATCH_OTHER_SOURCES    true     // if true the polling on M27 report is always active. Case: SD print start not from TFT35
#endif

/**
 * Home before power loss recovery
 * Many printer will crash printed model when homing, which is not suitable for home before PLR.
 * This function is suitable for Delta Printer.
 */
//#define HOME_BEFORE_PLR
  
#define M290_BABYSTEPPING                     // Support Babystepping M290 command

// Use M155 to report temperature instead M105 Polling ( Requires AUTO_REPORT_TEMPERATURES in Configuration_adv.h )
#define M155_AUTOREPORT

// Support M118 Actions command 
#define M118_ACTION_COMMAND

// Use M115 with EXTENDED_CAPABILITIES_REPORT and other strategies to check the config at runtime.
#define RUNTIME_CONFIG_VALIDATE

// User QRCode for extended reporting ( errors, info etch )
#define USE_QRCODE

#endif
