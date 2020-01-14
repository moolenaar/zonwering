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
#include <stdbool.h>
#include "lcd.h"
#include "font.h"
#include "kernel.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "display.h"

static char EEMEM ProgressOutline[20] = "[==============]";
static char EEMEM ProgressLineSolid[2] = "/";
static char EEMEM ProgressLineBlank[2] = " ";
static char EEMEM Up[10] = " Up ";
static char EEMEM Down[10] = " Down ";
static char EEMEM Actual[10] = "Actual";
static char EEMEM Set[10] = "Set";

uint8_t openPercent = 0;
uint8_t current;

static void ProgressBarSetup(void)
{
   WriteStaticString(lines5x12, 2, 2, ProgressOutline);
   current = 0;
}

static void ProgressBar(uint8_t targetValue)
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

static void ProgressPercent(void)
{
   char buffer[9];

   WriteString(font6x10, 0, 26, int32ToStr(buffer, 3, openPercent));
   WriteString(font6x10, 30, 26, int32ToStr(buffer, 3, MotorProgress()));
}

static void InvertedWhenMoving(void)
{
   switch(GetMotorDirection())
   {
      case DIRECTION_UP:
         WriteInverted8PixelString(font5x8, 13, 40, Up);
         WriteStaticString(font5x8, 50, 40, Down);
         break;

      case DIRECTION_DOWN:
         WriteStaticString(font5x8, 13, 40, Up);
         WriteInverted8PixelString(font5x8, 50, 40, Down);
         break;

      case DIRECTION_STOP:
      default:
         WriteStaticString(font5x8, 13, 40, Up);
         WriteStaticString(font5x8, 50, 40, Down);
         break;

   }
}
void mainScreenInit(void)
{
   char buffer[9];

   Clear();
   ProgressBarSetup();
   ProgressPercent();
   WriteStaticString(font5x8, 13, 40, Up);
   WriteStaticString(font5x8, 50, 40, Down);
   WriteStaticString(font5x8, 10, 16, Set);
   WriteStaticString(font5x8, 40, 16, Actual);
}

void mainScreenKey(enum PressedButtonState key)
{
   switch (key)
   {
      case PressedButtonDown:
         MotorOpen();
         break;

      case PressedButtonDownKey:
         if (openPercent < 100) openPercent += 25;
         MotorOpenPercent(openPercent);
         break;

      case PressedButtonUp:
         MotorClose();
         break;

      case PressedButtonUpKey:
         if (openPercent >= 25) openPercent -= 25;
         MotorOpenPercent(openPercent);
         break;

      case PressedButtonMenuRepeat:
         break;

      case PressedButtonMenuKey:
         SetScreenMode(ModeAskClosingTimeInit);
         break;

      case PressedButtonNone:
         MotorStop();
         break;

      default:
         break;
   }
}

void mainScreenUpdate(void)
{
   static uint8_t selection = 0;

   selection++;

   switch(selection % 20)
   {
      case 5:
         ProgressPercent();
         break;

      case 15:
         InvertedWhenMoving();
         break;

      default:
         ProgressBar(MotorProgress());
         break;
   };
}
