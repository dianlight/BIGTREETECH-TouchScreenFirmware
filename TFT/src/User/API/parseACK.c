#include "parseACK.h"
#include <ctype.h>
//#include "logger.h"

char *ack_rev_buf;
static u16 ack_index=0;

static char ack_seen(const char *str)
{
  u16 i;	
  for(ack_index=0; ack_index<ACK_MAX_SIZE && ack_rev_buf[ack_index]!=0; ack_index++)
  {
    for(i=0; str[i]!=0 && ack_rev_buf[ack_index+i]!=0 && ack_rev_buf[ack_index+i]==str[i]; i++)
    {}
    if(str[i]==0)
    {
      ack_index += i;      
      return true;
    }
  }
  return false;
}


static char ack_seen_ic(const char *str)
{
  u16 i;
  for(ack_index=0; ack_index<ACK_MAX_SIZE && ack_rev_buf[ack_index]!=0; ack_index++)
  {
    for(i=0; str[i]!=0 && ack_rev_buf[ack_index+i]!=0 && tolower(ack_rev_buf[ack_index+i])==tolower(str[i]); i++)
    {}
    if(str[i]==0)
    {
      ack_index += i;
      return true;
    }
  }
  return false;
}


static char ack_cmp(const char *str)
{
  u16 i;
  for(i=0; i<ACK_MAX_SIZE && str[i]!=0 && ack_rev_buf[i]!=0; i++)
  {
    if(str[i] != ack_rev_buf[i])
    return false;
  }
  if(ack_rev_buf[i] != 0) return false;
  return true;
}


static float ack_value()
{
  return (strtod(&ack_rev_buf[ack_index], NULL));
}

// Read the value after the / if exists
static float ack_second_value()
{
  char *secondValue = strchr(&ack_rev_buf[ack_index],'/');
  if(secondValue != NULL)
  {
    return (strtod(secondValue+1, NULL));
  }
  else 
  {
    return -0.5;
  }
}

void ackPopupInfo(const char *info)
{
  popupReminder((u8* )info, (u8 *)ack_rev_buf + ack_index);
}

void parseACK(void)
{
//  if(infoHost.rx_ok != true) return;      //not get response data
  if(infoHost.connected == false)         //not connected to Marlin
  {
    if(!ack_seen(connectmagic) && !ack_seen("ok")) return;
    infoHost.connected = true;
  }    

  // GCode command response
  bool gcodeProcessed = false;
  if(requestCommandInfo.inWaitResponse && ack_seen(requestCommandInfo.startMagic))
  {
    requestCommandInfo.inResponse = true;
    requestCommandInfo.inWaitResponse = false;
    gcodeProcessed = true;
  }
  if(requestCommandInfo.inResponse)
  {
    if(requestCommandInfo.cmd_rev_buf_pos < ACK_MAX_LINE)
    {
        requestCommandInfo.cmd_rev_buf[requestCommandInfo.cmd_rev_buf_pos++]=ack_rev_buf;
    }
    else 
    {
        closeRequestCommandInfo(false);
        ack_index = 0;
        ackPopupInfo(errormagic);
    }
    gcodeProcessed = true;
  }
  if(requestCommandInfo.inResponse && ack_seen(requestCommandInfo.errorMagic ))
  {
    closeRequestCommandInfo(false);
    gcodeProcessed = true;
  }
  if(requestCommandInfo.inResponse &&  ack_seen(requestCommandInfo.stopMagic ))
  {
    closeRequestCommandInfo(true);
    gcodeProcessed = true;
  }
  // end 

  // Async Gcode Callback
  if(requestCommandInfo.asyncCallback != NULL)
  {
    for(int c=0; c < CMD_ASYNC; c++)
    {
      (*requestCommandInfo.asyncCallback[c])(ack_rev_buf);
    }
  }
  // End

  //debugfixed(11,"W%x %.30s", infoHost.wait,ack_rev_buf);


  if(ack_cmp("ok\r\n") || ack_cmp("ok\n"))
  {
    infoHost.wait = false;	
   // debugfixed(12,"-New HW:%d Bw:%d Tw:%d", infoHost.wait, heatGetIsWaiting(BED), heatGetIsWaiting(0));
  }
  else
  {
    if(ack_seen("ok"))
    {
      infoHost.wait=false;
     // debugfixed(12,"+New HW:%d Bw:%d Tw:%d", infoHost.wait, heatGetIsWaiting(BED), heatGetIsWaiting(0));
    }					
    if(ack_seen("T:")) 
    {
      heatSetCurrentTemp(heatGetCurrentToolNozzle(), ack_value()+0.5);
      heatSetTargetTemp(heatGetCurrentToolNozzle(), ack_second_value()+0.5);
      for(TOOL i = BED; i < HEATER_NUM; i++)
      {
        if(ack_seen(toolID[i])) 
        {
          heatSetCurrentTemp(i, ack_value()+0.5);
          heatSetTargetTemp(i, ack_second_value()+0.5);
        }
      
      }
    }
    else if(ack_seen("B:"))		
    {
      heatSetCurrentTemp(BED,ack_value()+0.5);
      heatSetTargetTemp(BED, ack_second_value()+0.5);
    }
    else if(ack_seen(echomagic) && ack_seen(busymagic) && ack_seen("processing"))
    {
      busyIndicator(STATUS_BUSY);
    }
#ifdef ONBOARD_SD_SUPPORT     
    else if(ack_seen(bsdnoprintingmagic) && infoMenu.menu[infoMenu.cur] == menuPrinting)
    {
      infoHost.printing = false;
      completePrinting();
    }
    else if(ack_seen(bsdprintingmagic))
    {
      if(infoHost.printing && infoMenu.menu[infoMenu.cur] != menuPrinting && !infoHost.printing) {
          infoMenu.menu[++infoMenu.cur] = menuPrinting;
          infoHost.printing=true;
      }
      // Parsing printing data
      // Exampre: SD printing byte 123/12345
      char *ptr;
      u32 position = strtol(strstr(ack_rev_buf,"byte ")+5, &ptr, 10); 
      setPrintCur(position);
//      powerFailedCache(position);
    }    
#endif    
    else if(ack_seen(errormagic))
    {
        ackPopupInfo(errormagic);
    }
    else if(ack_seen(busymagic))
    {
        ackPopupInfo(busymagic);
    }
    else if(ack_seen(echomagic) && !gcodeProcessed)
    {
        ackPopupInfo(echomagic);
    }

  #ifdef M118_ACTION_COMMAND
    else if(ack_seen(commentmagic) && ack_seen_ic(actioncommandmagic))
    {
        if(!parseHostAction(&ack_rev_buf[ack_index]))
          ackPopupInfo("Unknown ACTION");
    }
  #endif
 }  
}

void parseACKml(void)
{
  ack_rev_buf = Serial_ReadLn();
  while(ack_rev_buf != NULL)
  {
    parseACK();
    loopProcess();
    ack_rev_buf = Serial_ReadLn();
  }
}



