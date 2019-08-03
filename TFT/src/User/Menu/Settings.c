#include "Settings.h"
#include "includes.h"
#include "Popup.h"


SETTINGS infoSettings;

#ifdef MARLIN2_AUTOCONFIG
  #define STARTLINE  LCD_HEIGHT/2 - (BYTE_HEIGHT*4) 
#else
  #define STARTLINE  LCD_HEIGHT/2 - BYTE_HEIGHT
#endif

#ifdef RUNTIME_CONFIG_VALIDATE 
#define ERRORS_STRINGS                                                     \
  E(CHECK_UUID, "[p1] TFT / Marlin Printer UUID not match! ")              \
  E(CHECK_EXTRUDER_NUM, "[p2] Number of extruders don't match! ")          \
  E(CHECK_M27_AUTOREPORT, "[p3] M27 is not enabled on Marlin firmware ")   \
  E(CHECK_ONBOARD_SD_SUPPORT, "[p4] Marlin firmware don't support SD ")    \
  E(CHECK_M155_AUTOREPORT, "[p6] M155 is not enabled on Marlin firmware ") \
  E(CHECK_PROMPT_SUPPORT, "[p7] HOST_PROMPT_SUPPORT is not enabled on Marlin firmware ")

#define E(x, y) x,
enum
{
  ERRORS_STRINGS
  TOTAL_CHECKS
};
#undef E
#define E(x, y) y,
const char *const error_desc[] = {ERRORS_STRINGS};
#undef E

// See https://github.com/ricmoo/QRCode
#define QR_TYPE 5 // 37x37
#define QR_SIZE 154

char CHECKS[TOTAL_CHECKS];

bool displayCheck(u16 startX)
{
   bool ok = true;
  char checks[14 + TOTAL_CHECKS + 6];
  memset(checks, 0, sizeof(checks));
  char errorMessage[QR_SIZE]; // qrcode type 4 max = 114
  memset(errorMessage, 0, sizeof(errorMessage));
  strcpy(checks, "Fw check:  ");
  for (int i = 0; i < TOTAL_CHECKS; i++)
   {
    if (CHECKS[i] == 0)
    {
      GUI_SetColor(RED);
      checks[10 + i] = 'E';
      int l = strlen(errorMessage);
      if (l - QR_SIZE > strlen(error_desc[i]))
      {
        if (l > 0)
          strcat(errorMessage, "\n");
        strncat(errorMessage, error_desc[i], QR_SIZE - l);
      }
      else
      {
        strncat(errorMessage, "...", QR_SIZE - l);
      }
        ok = false;
     }
    else if (CHECKS[i] == 1)
    {
      checks[13 + i] = '.';
   }
    else
    {
      checks[13 + i] = '!';
    }
  }

  if (ok)
  {
    strcat(checks, " OK");
     }
  else
  {
    strcat(checks, " ERROR");
    #ifdef USE_QRCODE
    GUI_DrawQRCode(LCD_WIDTH, LCD_HEIGHT, -1, QR_TYPE, (u8 *)errorMessage);
    #endif
   }
  GUI_ClearRect(0, STARTLINE + (BYTE_HEIGHT * 4), LCD_WIDTH, STARTLINE + (BYTE_HEIGHT * 5) + BYTE_HEIGHT);
  GUI_DispString(startX, STARTLINE + (BYTE_HEIGHT * 4), (u8 *)checks, 0);
  GUI_SetColor(FK_COLOR);

   return ok;
}

void checkOK(int check)
{
  CHECKS[check] = 1;
}

void checkError(int check)
{
  CHECKS[check] = 0;
}
#endif

SCROLL uuidScroll;


void menuInfo(void)
{
  const char* hardware = "Board   : BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = "Firmware: "HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;
  
  u16 HW_X = (LCD_WIDTH - my_strlen((u8 *)hardware)*BYTE_WIDTH)/2;
  u16 FW_X = (LCD_WIDTH - my_strlen((u8 *)firmware)*BYTE_WIDTH)/2;
  u16 centerY = LCD_HEIGHT/2;
  u16 startX = MIN(HW_X, FW_X);
  
  GUI_Clear(BLACK);

  GUI_DispString(startX, centerY - BYTE_HEIGHT, (u8 *)hardware, 0);
  GUI_DispString(startX, centerY, (u8 *)firmware, 0);

  while(!isPress()) loopProcess();
  while(isPress())  loopProcess();

  infoMenu.cur--;
}

void menuCheck(void)
{
  const char* hardware = "Board   : BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = "Firmware: "HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;
  
  u16 HW_X = (LCD_WIDTH - my_strlen((u8 *)hardware)*BYTE_WIDTH)/2;
  u16 FW_X = (LCD_WIDTH - my_strlen((u8 *)firmware)*BYTE_WIDTH)/2;
  u16 startX = MIN(HW_X, FW_X);


  GUI_Clear(BLACK);

  GUI_DispString(startX, STARTLINE, (u8 *)hardware, 0);
  GUI_DispString(startX, STARTLINE + BYTE_HEIGHT, (u8 *)firmware, 0);
 #ifdef MARLIN2_AUTOCONFIG
  GUI_DispString(startX, STARTLINE + (BYTE_HEIGHT * 2), (u8 *)"Printer : " MARLIN_VERSION " ", 0);
 #endif 
  GUI_DispString(startX, STARTLINE + (BYTE_HEIGHT * 4), (u8 *)"Fw check: (Pending....)", 0);
  M115_CAP *myCap = async_M115();
  memset(CHECKS, 3, sizeof(CHECKS));
  if (myCap->lastUpdateTime != 0)
  {
  #ifdef MACHINE_UUID
    GUI_RECT uuidRectC = {startX + (BYTE_WIDTH * 10), STARTLINE + (BYTE_HEIGHT * 3), LCD_WIDTH - (startX / 2) , STARTLINE + (BYTE_HEIGHT * 3) + BYTE_HEIGHT};
    if (strstr(myCap->_UUID, MACHINE_UUID) == NULL)
    {
      GUI_SetColor(RED);
      checkError(CHECK_UUID);
      GUI_DispString(startX, STARTLINE + (BYTE_HEIGHT * 3), (u8 *)"PrintUID: ", 0);
      GUI_SetColor(FK_COLOR);
      Scroll_CreatePara(&uuidScroll, (u8 *)MACHINE_UUID, &uuidRectC);
    }
    else
    {
      checkOK(CHECK_UUID);
      GUI_DispString(startX, STARTLINE + (BYTE_HEIGHT * 3), (u8 *)"PrintUID: ", 0);
      GUI_SetColor(FK_COLOR);
      Scroll_CreatePara(&uuidScroll, (u8 *)myCap->_UUID, &uuidRectC);
    }
  #endif
    if (myCap->_EXTRUDER_COUNT == EXTRUDER_NUM)
      checkOK(CHECK_EXTRUDER_NUM);
    else
      checkError(CHECK_EXTRUDER_NUM);
  #ifdef M27_AUTOREPORT
    if (myCap->_AUTOREPORT_SD_STATUS)
      checkOK(CHECK_M27_AUTOREPORT);
    else
      checkError(CHECK_M27_AUTOREPORT);
  #endif
  #ifdef ONBOARD_SD_SUPPORT
    if (myCap->_AUTOREPORT_SD_STATUS)
      checkOK(CHECK_ONBOARD_SD_SUPPORT);
    else
      checkError(CHECK_ONBOARD_SD_SUPPORT);
  #endif
  #ifdef M155_AUTOREPORT
    if (myCap->_AUTOREPORT_TEMP)
      checkOK(CHECK_M155_AUTOREPORT);
    else
      checkError(CHECK_M155_AUTOREPORT);
  #endif
#ifdef M118_ACTION_COMMAND
    if (myCap->_PROMPT_SUPPORT)
      checkOK(CHECK_PROMPT_SUPPORT);
    else
      checkError(CHECK_PROMPT_SUPPORT);
  #endif

  displayCheck(startX);
  }
  else 
  {
    // PopUp error.
    popupDrawPage(&bottomSingleBtn, (u8 *)"Check Error", (u8 *)"Unable to perform check", textSelect(LABEL_CONFIRM), NULL); // FIXME: Use a language string
    if (infoMenu.menu[infoMenu.cur] != menuPopup)
    {
      myCap->lastUpdateTime=OS_GetTime();
      infoMenu.menu[++infoMenu.cur] = menuPopup;
    }
  }
  GUI_SetColor(FK_COLOR);
  while (!isPress())
  {
   #if defined MACHINE_UUID || defined RUNTIME_CONFIG_VALIDATE
    if (infoMenu.menu[infoMenu.cur] != menuPopup)
      Scroll_DispString(&uuidScroll, 1, LEFT);
   #endif 
    loopProcess();
  } 
  while (isPress())
  {
   #if defined MACHINE_UUID || defined RUNTIME_CONFIG_VALIDATE
    if (infoMenu.menu[infoMenu.cur] != menuPopup)
      Scroll_DispString(&uuidScroll, 1, LEFT);
   #endif 
    loopProcess();
  }  
 
   infoMenu.cur--;
 }

void menuDisconnect(void)
{
  GUI_Clear(BLACK);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO), 0);

//  GPIOA->CRH &= 0xFFFFF00F;
//  GPIOA->CRH |= 0x00000440;// PA9/PA10

//  while(!isPress());
//  while(isPress());
//  GPIOA->CRH &= 0xFFFFF00F;
//  GPIOA->CRH |= 0x000008B0;
  infoMenu.cur--;
}


MENUITEMS settingsItems = {
//   title
LABEL_SETTINGS,
// icon                       label
 {{ICON_POWER_OFF,            LABEL_POWER_OFF},
  {ICON_LANGUAGE,             LABEL_LANGUAGE},
  {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
#ifdef RUNTIME_CONFIG_VALIDATE  
  {ICON_CHECK_CONFIG,         LABEL_CHECK_CONFIG},
#else
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
#endif  
  {ICON_BACK,                 LABEL_BACK},}
};


#define ITEM_BAUDRATE_NUM 2
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
//   icon                       label
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_250000},
};
const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200,250000};
static u8  item_baudrate_i = 0;


void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }	
  menuDrawPage(&settingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_0:
      mustStoreCmd("M81\n");
      break;
      
      case KEY_ICON_1: 
        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_2:    
        TSC_Calibration();
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;
      
      case KEY_ICON_5:
        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;                
        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.baudrate = item_baudrate[item_baudrate_i];
        Serial_Config(infoSettings.baudrate);
        break;

      #ifdef RUNTIME_CONFIG_VALIDATE 
      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuCheck;
        break;
      #endif

      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    loopProcess();		
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
