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
#include "clock.h"
#include "nonvolataile.h"

static char EEMEM Up[10] = " Up ";
static char EEMEM Down[10] = " Down ";
static char EEMEM Actual[10] = "Actual";
static char EEMEM Set[10] = "Set";

uint8_t openPercent;
bool block;

void MainScreenSetup(void)
{
   openPercent = nvGetOpenPercent();
}

static void ProgressPercent(void)
{
   char buffer[6];

   WriteString(font6x10, 8, 26, UtoaRightAligned(openPercent, buffer));
   WriteString(font6x10, 50, 26, UtoaRightAligned(MotorProgress(), buffer));
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

void MainScreenInit(void)
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

void MainScreenKey(enum PressedButtonState key)
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

void MainScreenUpdate(void)
{
   static uint8_t selection = 0;

   selection++;

   switch(selection % 5)
   {
      case 0:
         ProgressPercent();
         break;

      case 1:
         InvertedWhenMoving();
         break;

      case 2:
         if ((GetUpDownTime() == 0) && (GetMotorDirection() == DIRECTION_STOP))
         {
            openPercent = 0;
         }
         break;

      case 3:
            nvSetOpenPercent(openPercent);
         break;

      default:
         ProgressBar(MotorProgress());
         break;
   };
}
