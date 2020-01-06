/* mainscreen.c
 * Copyright 2020 Herman Moolenaar
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
#include "lcd.h"
#include "font.h"
#include "kernel.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "display.h"

static char EEMEM ProgressOutline[20] = "[==============]";
static char EEMEM ProgressLine[2] = "/ ";

uint8_t openPercent = 0;
uint8_t current;

static void ProgressBarSetup(void)
{
   WriteStaticString(lines5x12, 2, 2, ProgressOutline);
}

static void ProgressBar(uint8_t percent)
{
   percent = (uint16_t)percent * 77 / 100;
   if(percent > current)
   {
      for (uint8_t i = current; i < percent; ++i)
      {
         WriteStaticString(lines5x12, 3 + i, 2, &ProgressLine[0]);
      }
   }
   else if (percent < current)
   {
      for (uint8_t i = current; i > percent; --i)
      {
         WriteStaticString(lines5x12, 3 + i, 2, &ProgressLine[1]);
      }
   }

   current = percent;
}

void mainScreenInit(void)
{
   char buffer[9];

   Clear();
   ProgressBarSetup();
   WriteString(font6x10, 3, 18, int32ToStr(buffer, 3, 0));
}

void mainScreenUpdate(void)
{
   ProgressBar(0);

   switch (GetButtonPressed())
   {
      case PressedButtonMenu:
         SetScreenMode(ModeAskClosingTimeInit);
         break;      

      case PressedButtonDown:
         if (openPercent < 100) openPercent += 25; 
         break;

      case PressedButtonUp:
         if (openPercent >0) openPercent -= 25; 
         break;

      default:
         switch (GetButtonState())
         {
            case PressedButtonNone:
               SetMotorDirection(DIRECTION_STOP);
               break;

            case PressedButtonDownRepeat:
               SetMotorDirection(DIRECTION_DOWN);
               break;

            case PressedButtonUpRepeat:
               SetMotorDirection(DIRECTION_UP);
               break;

            default:
               break;
         }
         break;
   }
}
