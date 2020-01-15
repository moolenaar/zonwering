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
#include "display.h"
#include "lcd.h"
#include "font.h"
#include "kernel.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "diagnosticscreen.h"
#include "closingtimescreen.h"
#include "mainscreen.h"

uint8_t screen = 0;

static char EEMEM ProductTitle[20] = "Sun Blind Control";
static char EEMEM ProductVersion[20] = "version 1.0";

static void displayProductTitle(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 10, ProductTitle);
   WriteStaticString(font5x8, 10, 24, ProductVersion);
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
         mainScreenInit();
         SetKeyHandler(mainScreenKey);
         screen = ModeMainScreenUpdate;
         break;

      case ModeMainScreenUpdate:
         // update main screen
         mainScreenUpdate();
         break;

      case ModeAskClosingTimeInit:
         // ask user to input time before closing the sun blinds
         closingTimeInit();
         SetKeyHandler(ClosingTimeKey);
         screen = ModeAskClosingTimeUpdate;
         break;

      case ModeAskClosingTimeUpdate:
         // update the input time screen
         closingTimeUpdate();
         break;

      case ModeAskFullOpenInit:
         // display screen to input time to open sun blinds 100%
         Clear();
         screen = ModeAskFullOpenUpdate;
         break;

      case ModeAskFullOpenUpdate:
         // screen to input 100% open time is displayed
         break;

      case ModeDiagnosticInit:
         // display diagnostic screen
         diagnosticInit();
         screen = ModeDiagnosticUpdate;
         break;

      case ModeDiagnosticUpdate:
         // diagnostic screen is displayed
         diagnosticUpdate();
         break;

      default:
         screen = ModeProductTitle;
         break;
   }
}

