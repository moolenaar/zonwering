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
#include <stdlib.h>
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
bool block;

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

static char *utoaRightAligned(uint8_t value, char *buffer)
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

static void ProgressPercent(void)
{
   char buffer[6];

   WriteString(font6x10, 8, 26, utoaRightAligned(openPercent, buffer));
   WriteString(font6x10, 50, 26, utoaRightAligned(MotorProgress(), buffer));
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
   block = false;
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
         if (GetMotorDirection() == DIRECTION_UP)
         {
            MotorStop();
            block = true;
         }
         else
         {
            MotorOpen();
         }
         break;

      case PressedButtonDownKey:
         if (block)
         {
            MotorStop();
            block = false;
         }
         else
         {
            if (openPercent < 100) openPercent += 25;
            MotorOpenPercent(openPercent);
         }
         break;

      case PressedButtonUp:
         if (GetMotorDirection() == DIRECTION_DOWN)
         {
            MotorStop();
            block = true;
         }
         else
         {
            openPercent = 0;
            MotorClose();
         }
         break;

      case PressedButtonUpKey:
         if (block)
         {
            MotorStop();
            block = false;
         }
         else
         {
            if (openPercent >= 25) openPercent -= 25;
            MotorOpenPercent(openPercent);
         }
         break;

      case PressedButtonMenuKey:
         SetScreenMode(ModeAskClosingTimeInit);
         break;

      case PressedButtonMenuRepeat:
         SetScreenMode(ModeAskFullOpenInit);
         break;

      case PressedButtonNone:
         openPercent = 0;
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
