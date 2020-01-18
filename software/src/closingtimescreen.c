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
#include <stdlib.h>
#include "lcd.h"
#include "font.h"
#include "button.h"
#include "display.h"

/* delay time and step in minutes */
#define MAXDELAYTIME 720
#define STEPDELAYTIME 5

int8_t step = 0;

static char EEMEM WaitTitle1[20]  = "Waiting before";
static char EEMEM WaitTitle2[20]  = "closing shades...";
static char EEMEM Semicolon[2]   = ".";
static char EEMEM Hour[2]   = "h";

static char *utoa2Digits(uint8_t value, char *buffer)
{
   utoa(value, buffer, 10);
   if (buffer[1] == 0)
   {
      buffer[1] = buffer[0];
      buffer[0] = '0';
      buffer[2] = 0;
   }
   return buffer;
}

static void updateTime(void)
{
   char buffer[6];

   WriteString(font6x10, 5, 20, utoa2Digits(GetTime() / 60, buffer));
   WriteString(font6x10, 28, 20, utoa2Digits(GetTime() % 60, buffer));
}

void closingTimeInit(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 0, WaitTitle1);
   WriteStaticString(font5x8, 0, 8, WaitTitle2);
   WriteStaticString(font6x10, 23, 20, Semicolon);
   WriteStaticString(font5x8, 46, 23, Hour);
}

void ClosingTimeKey(enum PressedButtonState key)
{
   uint16_t delayTime = GetTime();

   delayTime = (delayTime / STEPDELAYTIME) * STEPDELAYTIME + (delayTime % STEPDELAYTIME > 0) ? STEPDELAYTIME : 0;

   switch (key)
   {
      case PressedButtonDownKey:
         if (delayTime < MAXDELAYTIME - STEPDELAYTIME)
         {
            delayTime += STEPDELAYTIME;
         }
         else
         {
            delayTime = MAXDELAYTIME;
         }
         MotorDelayClose(delayTime);
         break;

      case PressedButtonUpKey:
         if (delayTime > STEPDELAYTIME)
         {
            delayTime -= STEPDELAYTIME;
         }
         else
         {
            delayTime = 0;
         }
         MotorDelayClose(delayTime);
         break;

      case PressedButtonMenuKey:
         SetScreenMode(ModeMainScreenInit);
         break;

      case PressedButtonNone:
         step = 0;
         break;

      default:
         break;
   }
}

void closingTimeUpdate(void)
{
   updateTime();
}
