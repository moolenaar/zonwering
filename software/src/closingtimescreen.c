/* closingtimescreen.c
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
#include <stdbool.h>
#include "lcd.h"
#include "font.h"
#include "button.h"
#include "diagnosticscreen.h"
#include "display.h"

uint16_t delayTime = 0;

static char EEMEM WaitTitle1[20]  = "Waiting before";
static char EEMEM WaitTitle2[20]  = "closing blinds...";
static char EEMEM Semicolon[2]   = ".";
static char EEMEM Hour[2]   = "h";

static void updateTime(void)
{
   char buffer[9];

   WriteString(font6x10, 2, 20, int32ToStr(buffer, 2, delayTime / 60));
   WriteString(font6x10, 27, 20, int32ToStr(buffer, 2, delayTime % 60));

}

void closingTimeInit(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 0, WaitTitle1);
   WriteStaticString(font5x8, 0, 8, WaitTitle2);
   WriteStaticString(font6x10, 25, 20, Semicolon);
   WriteStaticString(font5x8, 60, 20, Hour);
}

void ClosingTimeKey(enum PressedButtonState key)
{
   switch (key)
   {
      case PressedButtonDownKey:
         if (delayTime < 720 - 30)
         {
            delayTime += 30;
         }
         else
         {
            delayTime = 720;
         }
         break;

      case PressedButtonUpKey:
         if (delayTime > 30)
         {
            delayTime -= 30;
         }
         else
         {
            delayTime = 0;
         }
         break;

      case PressedButtonMenuKey:
         SetScreenMode(ModeMainScreenInit);
         break;

      default:
         break;
   }
   updateTime();
}

void closingTimeUpdate(void)
{
   updateTime();
}
