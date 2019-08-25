#include "MainPage.h"
#include "includes.h"

//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ)
const MENUITEMS mainPageItems = {
//   title
LABEL_READY,
// icon                       label
 {{ICON_HEAT,                 LABEL_HEAT},
  {ICON_MOVE,                 LABEL_MOVE},
  {ICON_HOME,                 LABEL_HOME},
  {ICON_PRINT,                LABEL_PRINT},
  {ICON_EXTRUDE,              LABEL_EXTRUDE},
  {ICON_FAN,                  LABEL_FAN},
  {ICON_SETTINGS,             LABEL_SETTINGS},
  {ICON_LEVELING,             LABEL_LEVELING},}
};

void menuMain(void)
{
  KEY_VALUES key_num=KEY_IDLE;

  menuDrawPage(&mainPageItems);

  while(infoMenu.menu[infoMenu.cur] == menuMain)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHeat;      break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove;      break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuHome;      break;
      case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuPrint;     break;      
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuExtrude;   break;
      case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuFan;       break;
      case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;  break;
      case KEY_ICON_7: 
        storeCmd("G28\n");
        storeCmd("G29\n");
// TODO: Manual level support
//  Disconnected.
/*
Connecting...
Printer is now online.
>>> G29 S1
SENDING:G29 S1
>>> G29 S2
SENDING:G29 S2
MBL G29 point 2 of 9
>>> G29 S23
SENDING:G29 S23
S out of range (0-5).
>>> G29 S3
SENDING:G29 S3
J not entered.
>>> G29 S2
SENDING:G29 S2
MBL G29 point 3 of 9
>>> G29 S3
SENDING:G29 S3
J not entered.
>>> G29 S2
SENDING:G29 S2
MBL G29 point 4 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 5 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 6 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 7 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 8 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 9 of 9
>>> G29 S2
SENDING:G29 S2
Mesh probing done.
MBL G29 point -1 of 9
>>> G29 S2
SENDING:G29 S2
Start mesh probing with "G29 S1" first.
>>> M500
SENDING:M500
echo:Settings Stored (614 bytes; crc 31912)
>>> G29 S0
SENDING:G29 S0
Mesh Bed Leveling has no data.
>>> G29 S1
SENDING:G29 S1
>>> G29 S1
SENDING:G29 S1
>>> G29 S2
SENDING:G29 S2
MBL G29 point 2 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 3 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 4 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 5 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 6 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 7 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 8 of 9
>>> G29 S2
SENDING:G29 S2
MBL G29 point 9 of 9
>>> G29 S2
SENDING:G29 S2
Mesh probing done.
MBL G29 point -1 of 9
>>> M500
SENDING:M500
echo:Settings Stored (614 bytes; crc 31912)
//
Cap:AUTOLEVEL:0
Cap:Z_PROBE:0
Cap:LEVELING_DATA:1

*/

        break;
      default:break;
    }		
    loopProcess();
  }
}

void connectionCheck(void)
{
    static u32 nowTime = 0;
    if(infoHost.connected)return;
    if(OS_GetTime()<nowTime+300) return;

    if(storeCmd("M115\n")==false) return;

    nowTime=OS_GetTime();
}
