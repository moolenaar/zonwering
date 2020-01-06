/* diagnosticscreen.c
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
#include "display.h"
#include "lcd.h"
#include "font.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "diagnosticscreen.h"
#include "display.h"

static char EEMEM Voltage[10] = "Voltage:";
static char EEMEM Samples[10] = "Samples:";
static char EEMEM Button[10]  = "Button:";
static char EEMEM Up[9]       = "Up";
static char EEMEM Down[9]     = "Down";
static char EEMEM Up2[9]      = "Up 2";
static char EEMEM Down2[9]    = "Down 2";
static char EEMEM Menu[9]     = "Menu";
static char EEMEM Menu2[9]    = "Menu 2";
static char EEMEM None[9]     = "        ";

void diagnosticInit(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 0, Button);
   WriteStaticString(font5x8, 0, 8, Voltage);
   WriteStaticString(font5x8, 0, 16, Samples);
}

void diagnosticUpdate(void)
{
   char buffer[10];
   switch (GetButtonState())
   {
      case PressedButtonNone:
         WriteStaticString(font5x8, 40, 0, None);
         SetMotorDirection(DIRECTION_STOP);
         break;

      case PressedButtonDown:
         WriteStaticString(font5x8, 40, 0, Down);
         SetMotorDirection(DIRECTION_DOWN);
         break;

      case PressedButtonDownRepeat:
         WriteStaticString(font5x8, 40, 0, Down2);
         SetMotorDirection(DIRECTION_DOWN);
         break;

      case PressedButtonUp:
         WriteStaticString(font5x8, 40, 0, Up);
         SetMotorDirection(DIRECTION_UP);
         break;

      case PressedButtonUpRepeat:
         WriteStaticString(font5x8, 40, 0, Up2);
         SetMotorDirection(DIRECTION_UP);
         break;

      case PressedButtonMenu:
         WriteStaticString(font5x8, 40, 0, Menu);
         SetMotorDirection(DIRECTION_STOP);
         break;

      case PressedButtonMenuRepeat:
         WriteStaticString(font5x8, 40, 0, Menu2);
         SetMotorDirection(DIRECTION_STOP);
         break;
   }
   WriteString(font5x8, 40, 8, int32ToStr(buffer, 6, GetVoltage()));
   WriteString(font5x8, 40, 16, int32ToStr(buffer, 8, GetNrSamples()));
}
