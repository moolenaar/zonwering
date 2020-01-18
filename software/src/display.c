/* display.c
 * Copyright 2019 Herman Moolenaar
 * This file is part of zonwering.
 *
 * zonwering is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zonwering is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zonwering. If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/eeprom.h>
#include <stdlib.h>
#include "display.h"
#include "lcd.h"
#include "font.h"
#include "kernel.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "fullopen.h"
#include "closingtimescreen.h"
#include "mainscreen.h"
#include "idlescreen.h"
#include "clock.h"

/* time before switching off backlight; 100ms resolution */
#define BACKLIGHTTIMOUTTIME (10 * 60 * 5)

static uint8_t screen = 0;
static uint8_t current;
static uint16_t backlightTimer = BACKLIGHTTIMOUTTIME;

static char EEMEM ProductTitle1[20] = "Sun Shade";
static char EEMEM ProductTitle2[20] = "Control";
static char EEMEM ProductVersion[20] = "version 1.0";
static char EEMEM ProgressOutline[20] = "[==============]";
static char EEMEM ProgressLineSolid[2] = "/";
static char EEMEM ProgressLineBlank[2] = " ";

void ProgressBarSetup(void)
{
   WriteStaticString(lines5x12, 2, 2, ProgressOutline);
   current = 0;
}

void ProgressBar(uint8_t targetValue)
{
   targetValue = (uint16_t)targetValue * 77 / 100;
   if (targetValue >= current)
   {
      for (uint8_t i = current; i < targetValue; ++i)
      {
         WriteStaticString(lines5x12, 3 + i, 2, ProgressLineSolid);
      }
   }
   else if (targetValue < current)
   {
      for (uint8_t i = current; i > targetValue; --i)
      {
         WriteStaticString(lines5x12, 3 + i, 2, ProgressLineBlank);
      }
   }

   current = targetValue;
}

char *UtoaRightAligned(uint8_t value, char *buffer)
{
   utoa(value, buffer, 10);
   if (buffer[1] == 0)
   {
      buffer[2] = 0;
      buffer[1] = buffer[0];
      buffer[0] = ' ';

   }
   if (buffer[2] == 0)
   {
      buffer[3] = 0;
      buffer[2] = buffer[1];
      buffer[1] = buffer[0];
      buffer[0] = ' ';
   }
   return buffer;
}

static void displayProductTitle(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 8, ProductTitle1);
   WriteStaticString(font5x8, 0, 16, ProductTitle2);
   WriteStaticString(font5x8, 0, 24, ProductVersion);
   TaskSleep(2000);
}

enum ScreenModeType GetScreenMode(void)
{
   return screen;
}

void SetScreenMode(enum ScreenModeType screenMode)
{
   screen = screenMode;
}

void ResetBacklightTimer(void)
{
   backlightTimer = BACKLIGHTTIMOUTTIME;
}

void HandleDisplay(void)
{
   switch(screen)
   {
      case ModeProductTitle:
         // write product title and software version
         displayProductTitle();
         screen = ModeMainScreenInit;
         break;

      case ModeMainScreenInit:
         // display main screen with progress bar and percent blinds lowered
         MainScreenInit();
         SetKeyHandler(MainScreenKey);
         screen = ModeMainScreenUpdate;
         break;

      case ModeMainScreenUpdate:
         // update main screen
         MainScreenUpdate();
         break;

      case ModeAskClosingTimeInit:
         // ask user to input time before closing the sun blinds
         ClosingTimeInit();
         SetKeyHandler(ClosingTimeKey);
         screen = ModeAskClosingTimeUpdate;
         break;

      case ModeAskClosingTimeUpdate:
         // update the input time screen
         ClosingTimeUpdate();
         break;

      case ModeAskFullOpenInit:
         // display screen to input time to open sun blinds 100%
         Clear();
         FullOpenInit();
         SetKeyHandler(FullOpenKey);
         screen = ModeAskFullOpenUpdate;
         break;

      case ModeAskFullOpenUpdate:
         // screen to input 100% open time is displayed
         FullOpenUpdate();
         break;

      case ModeIdleInit:
         IdleScreenInit();
         SetKeyHandler(idleScreenKey);
         screen = ModeIdleUpdate;
         break;

      case ModeIdleUpdate:
         IdleScreenUpdate();
         break;

      default:
         screen = ModeProductTitle;
         break;
   }

   if ((GetMotorDirection() != DIRECTION_STOP) || (GetTime() > 0))
   {
      ResetBacklightTimer();
   }

   if (backlightTimer > 0)
   {
      backlightTimer--;
      if (backlightTimer == 0)
      {
         screen = ModeIdleInit;
      } 
   }
}

