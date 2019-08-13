#ifndef _AUTO_CONFIGRATION_H_
#define _AUTO_CONFIGRATION_H_

    #ifdef MARLIN2_AUTOCONFIG
        #define _XSTR(x) #x
        #define STR(x) _XSTR(x)

//        #define __STM32F1__ // Bogus value to evade error


        #include STR(MARLIN2_HOME/Marlin/src/core/macros.h)

        // Undefine same name in Marlin config
        #undef X_MAX_POS
        #undef Y_MAX_POS
        #undef Z_MAX_POS

        #include STR(MARLIN2_HOME/Marlin/Configuration.h)

        #define HAS_DRIVER(T) ENABLED(T)   // BOGUS Value. All sub step driver configuration are fake
        #include STR(MARLIN2_HOME/Marlin/Configuration_adv.h)
        #include STR(MARLIN2_HOME/Marlin/src/inc/Version.h)
        #undef MAX
        #undef MIN
        #undef ABS
        #undef A

        #undef EXTRUDER_NUM
        #define EXTRUDER_NUM EXTRUDERS    //set in 1~6

        #undef HEAT_MAX_TEMP
        #define HEAT_MAX_TEMP    {BED_MAXTEMP,    HEATER_0_MAXTEMP,       HEATER_1_MAXTEMP,       HEATER_2_MAXTEMP,       HEATER_3_MAXTEMP,       HEATER_4_MAXTEMP,       HEATER_5_MAXTEMP}    //max temperature can be set

        #undef DEFAULT_SPEED_MOVE
        #define DEFAULT_SPEED_MOVE      DEFAULT_TRAVEL_ACCELERATION  //Move default speed  mm/min

        #if !defined SDSUPPORT && defined ONBOARD_SD_SUPPORT
            #error "ONBOARD_SD_SUPPORT Enabled but Marling config without SDSUPPORT"
        #elif defined SDSUPPORT && !defined ONBOARD_SD_SUPPORT
            #define ONBOARD_SD_SUPPORT
        #endif

        #if !defined AUTO_REPORT_SD_STATUS && defined M27_AUTOREPORT
            #error "M27_AUTOREPORT Enabled but Marlin config without AUTO_REPORT_SD_STATUS"
        #elif defined AUTO_REPORT_SD_STATUS && !defined M27_AUTOREPORT
            #define M27_AUTOREPORT
        #endif

        #if !defined AUTO_REPORT_TEMPERATURES && defined M155_AUTOREPORT
            #error "M155_AUTOREPORT Enabled but Marling config without AUTO_REPORT_TEMPERATURES"
        #elif defined AUTO_REPORT_TEMPERATURES && !defined M155_AUTOREPORT
            #define M155_AUTOREPORT
        #endif

        #if !defined EXTENDED_CAPABILITIES_REPORT && defined RUNTIME_CONFIG_VALIDATE
            #error "RUNTIME_CONFIG_VALIDATE Enabled but Marlin config without EXTENDED_CAPABILITIES_REPORT"
        #elif defined EXTENDED_CAPABILITIES_REPORT && !defined RUNTIME_CONFIG_VALIDATE
            #define RUNTIME_CONFIG_VALIDATE
        #endif

        #if !defined BABYSTEPPING && defined M290_BABYSTEPPING
            #error "M290_BABYSTEPPING Enabled but Marlin config without BABYSTEPPING"
        #elif defined BABYSTEPPING && !defined M290_BABYSTEPPING
            #define M290_BABYSTEPPING
        #endif

        #if !defined BIGTREETECH_TFT_LCD && defined M118_ACTION_COMMAND
            #error "M118_ACTION_COMMAND Enabled but Marlin config without BIGTREETECH_TFT_LCD"
        #elif defined BIGTREETECH_TFT_LCD && !defined M118_ACTION_COMMAND
            #define M118_ACTION_COMMAND
        #endif

        #define MARLIN_VERSION "Marlin/" SHORT_BUILD_VERSION



    #endif
#endif
