#include "host_actions.h"

// Supported Action https://reprap.org/wiki/G-code#Action_commands
// Original Octoprint Action Commands
#define ACTION_PAUSE            "pause"                      // pause current print
#define ACTION_CANCEL           "cancel"                     // cancel the current printer job ( a message is optional )
//#define ACTION_DISCONNET        "disconnect"                 // disconnect
#define ACTION_PAUSED           "paused"                     // the job is paused
#define ACTION_RESUME           "resume"                     // resume the job
//#define ACTION_OUT_OF_FILAMENT  "out_of_filament T%d"        // out of filement on %d extruder_id
#define ACTION_RESUMED          "resumed"                    // the job is resumed

// Dialog commands  (https://reprap.org/wiki/G-code#M876:_Dialog_handling)
//#define ACTION_PROMPT_BEGIN  "prompt_begin %s"  // Starts the definition of a prompt dialog. <message> is the message to display to the user. (support: OctoPrint 1.3.9+ w/ enabled Action Command Prompt support plugin)
//#define ACTION_PROMPT_CHOICE "prompt_choice %s" // Defines a dialog choice with the associated <text>. (support: OctoPrint 1.3.9+ w/ enabled Action Command Prompt support plugin)
//#define ACTION_PROMPT_BUTTON "prompt_button %s" // Same as prompt_choice. (support: OctoPrint 1.3.9+ w/ enabled Action Command Prompt support plugin)
//#define ACTION_PROMPT_SHOW   "prompt_show"      // Tells the host to prompt the user with the defined dialog. (support: OctoPrint 1.3.9+ w/ enabled Action Command Prompt support plugin)
//#define ACTION_PROMPT_END    "prompt_end"       // Tells the host to close the dialog (e.g. the user made the choice through the printer directly instead of through the host). (support: OctoPrint 1.3.9+ w/ enabled Action Command Prompt support plugin)
//#define ACTION_PROBE_REWIPE  "probe_rewipe"     // Displays dialog indicating G29 Probing is Retrying. (support: Lulzbot Cura 3.6+)
//#define ACTION_PROBE_FAILED  "probe_failed"     // Cancels print job and displays dialog indicating G29 Probing failed (support: Lulzbot Cura 3.6+)

// New Action Commands define for the LCD.
#define ACTION_CONNECT   "connect"            // Tell to LCD that Marlin is ready to send action command
#define ACTION_MEDIA      "media"              // Inform on media change ( sub command are insert/remove/error)
#define ACTION_BELL       "bell %d %d"               // Bell or visual bell ( frequency duration)

#define ACTION_START         "start"                        // Print start
#define ACTION_PROGRESS      "progress"                     // Change printing progress (seconds, percent)
#define ACTION_PROGRESS_WF   ACTION_PROGRESS" S%d P%d"      // Change printing progress (seconds, percent)
#define ACTION_STATUS        "status"                       // Change status event ( message )


bool parseHostAction(char *action)
{
    if( strstr(action,ACTION_START) != NULL || strstr(action, ACTION_PROGRESS) != NULL)
    {
      if(!infoHost.printing && infoMenu.menu[infoMenu.cur] != menuPrinting && !infoHost.printing) {
          infoMenu.menu[++infoMenu.cur] = menuBeforePrinting;
          infoFile.source=SERIAL;
          infoHost.printing=true;
      }
      int position, time;
      if (sscanf(action,ACTION_PROGRESS_WF,&time,&position) != EOF)
      {
            setPrintSize(100);
            if(getPrintCur() > 0)
            {
                setPrintPause(false);
            }
            setPrintCur(position);
            setABSPrintingTime(time);
      }
      return true;
    }
    else if ( strstr(action,ACTION_STATUS) != NULL )
    {
        GUI_SetColor(GREEN);
        GUI_ClearRect(0,0,LCD_WIDTH,BYTE_HEIGHT);
        GUI_DispStringInRect(0,0,LCD_WIDTH,BYTE_HEIGHT,(u8 *)action,0);
        GUI_SetColor(FK_COLOR);
        return true;
    }
    else if ( strstr(action,ACTION_PAUSED) != NULL )
    {
        setPrintPause(true);
        return true;
    }
    else if ( strstr(action,ACTION_RESUMED) != NULL )
    {
        setPrintPause(false);
        return true;
    }
    else if ( strstr(action,ACTION_CANCEL) != NULL )
    {
        if(infoMenu.menu[infoMenu.cur] == menuPrinting)
        {
            haltPrinting();
            infoMenu.cur--;
        }
        return true;
    }
    else if ( strstr(action,ACTION_MEDIA) != NULL )
    {
        // do nothing for now
        return true;
    }
    else if ( strstr(action,ACTION_PAUSE) != NULL || strstr(action,ACTION_RESUME) != NULL )
    {
        return true;
    }
    return false;
}

void sendActionCommandPause(void)
{
    mustStoreCmd("M118 A1 action:"ACTION_PAUSE"\n");
}

void sendActionCommandResume(void)
{
    mustStoreCmd("M118 A1 action:"ACTION_RESUME"\n");
}

void sendActionCommandCancel(void)
{
    mustStoreCmd("M118 A1 action:"ACTION_CANCEL"\n");
}
