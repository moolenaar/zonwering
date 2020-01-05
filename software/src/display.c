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

uint8_t current;
uint8_t screen = 0;

static char EEMEM ProductTitle[20] = "Sun Blind Control";
static char EEMEM ProductVersion[20] = "version 1.0";
static char EEMEM ProgressOutline[20] = "[==============]";
static char EEMEM ProgressLine[2] = "/ ";
static char EEMEM Voltage[10] = "Voltage:";
static char EEMEM Samples[10] = "Samples:";
static char EEMEM Button[10] = "Button:";
static char EEMEM Up[9]    = "Up";
static char EEMEM Down[9]  = "Down";
static char EEMEM Up2[9]   = "Up 2";
static char EEMEM Down2[9] = "Down 2";
static char EEMEM Menu[9]  = "Menu";
static char EEMEM Menu2[9] = "Menu 2";
static char EEMEM None[9]  = "        ";


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

static char* int32ToStr(char* buffer, uint8_t before, int32_t value)
{
   int8_t i;
   uint32_t divide = 1;

   for (i = 1; i < before; i++) divide *= 10;

   if (value < 0)
   {
      buffer[0] = '-';
      value = -value;
   }
   else
   {
      buffer[0] = ' ';
   }

   for (i = 1; i <= before; i++)
   {
      buffer[i] = '0' + value / divide;
      value %= divide;
      divide /= 10;
   }

   buffer[before + 1] = 0;

   i = 0;
   do
   {
      i++;
   } while ((i < before) && (buffer[i] <= '0'));

   i--;

   while(i > 0)
   {
      buffer[i--] = buffer[0];
      buffer[0] = ' ';
   }

   return buffer;
}

static void displayProductTitle(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 10, ProductTitle);
   WriteStaticString(font5x8, 10, 24, ProductVersion);
   TaskSleep(2000);
}

static void mainScreenInit(void)
{
   char buffer[9];

   Clear();
   ProgressBarSetup();
   WriteString(font6x10, 3, 18, int32ToStr(buffer, 3, 0));
}

static void mainScreenUpdate(void)
{
   ProgressBar(0);
}

static void diagnosticInit(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 0, Button);
   WriteStaticString(font5x8, 0, 8, Voltage);
   WriteStaticString(font5x8, 0, 16, Samples);
}

static void diagnosticUpdate(void)
{
   char buffer[10];
   switch (GetButtonState())
   {
      case PressedButtonNone:
         WriteStaticString(font5x8, 40, 0, None);
//         SetMotorDirection(DIRECTION_STOP);
         break;

      case PressedButtonDown:
         WriteStaticString(font5x8, 40, 0, Down);
//         SetMotorDirection(DIRECTION_DOWN);
         break;

      case PressedButtonDownRepeat:
         WriteStaticString(font5x8, 40, 0, Down2);
//         SetMotorDirection(DIRECTION_DOWN);
         break;

      case PressedButtonUp:
         WriteStaticString(font5x8, 40, 0, Up);
//         SetMotorDirection(DIRECTION_UP);
         break;

      case PressedButtonUpRepeat:
         WriteStaticString(font5x8, 40, 0, Up2);
//         SetMotorDirection(DIRECTION_UP);
         break;

      case PressedButtonMenu:
         WriteStaticString(font5x8, 40, 0, Menu);
//         SetMotorDirection(DIRECTION_STOP);
         break;

      case PressedButtonMenuRepeat:
         WriteStaticString(font5x8, 40, 0, Menu2);
//         SetMotorDirection(DIRECTION_STOP);
         break;
   }
   WriteString(font5x8, 40, 8, int32ToStr(buffer, 6, GetVoltage()));
   WriteString(font5x8, 40, 16, int32ToStr(buffer, 8, GetNrSamples()));
}

//static char* floatToStr(char* buffer, uint8_t before, uint8_t after, float value)
//{
//   uint8_t i, pos, tmp;
//
//   int32ToStr(buffer, before, (int32_t)value);
//   pos = before + 1;
//
//   if (after > 0)
//   {
//      buffer[pos++] = '.';
//
//      value = value - (int32_t)value;
//      if (value < 0)
//      {
//         value = -value;
//      }
//
//      float divide = 0.1;
//
//      for (i = 0; i < after; i++)
//      {
//         tmp = (uint8_t)(value / divide);
//         buffer[pos + i] = '0' + tmp;
//         value -= tmp * divide;
//         divide /= 10;
//      }
//   }
//   buffer[pos + after] = 0;
//
//   return buffer;
//}

void DisplaySetup(void)
{
   LcdSetup();
}

enum ScreenModeType GetScreenMode(void)
{
   return screen;
}

void SetScreenMode(enum ScreenModeType screenMode)
{
   screen = screenMode;
}

void DisplayTask()
{
   LcdInitialize();

   while (true)
   {
      switch(screen)
      {
         case ModeProductTitle:
            // write product title and software version
            displayProductTitle();
            screen = ModeMainScreenInit;
            screen = ModeDiagnosticInit;
            break;

         case ModeMainScreenInit:
            // display main screen with progress bar and percent blinds lowered
            mainScreenInit();
            screen = ModeMainScreenUpdate;
            break;

         case ModeMainScreenUpdate:
            // update main screen
            mainScreenUpdate();
            break;

         case ModeAskClosingTimeInit:
            // ask user to input time before closing the sun blinds
            Clear();
            screen = ModeAskClosingTimeUpdate;
            break;

         case ModeAskClosingTimeUpdate:
            // update the input time screen
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

      }
      TaskSleep(200);
   }
}

