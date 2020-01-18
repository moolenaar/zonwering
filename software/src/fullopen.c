/* fullopen.c
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
#include "kernel.h"
#include "display.h"
#include "font.h"
#include "motor.h"
#include "button.h"
#include "fullopen.h"

static char EEMEM Accept[9]        = "Accept";
static char EEMEM Back[9]          = "Back";
static char EEMEM Title1[20]       = "Use current";
static char EEMEM Title2[20]       = "position as fully";
static char EEMEM Title3[20]       = "open?";
static char EEMEM PositionSet[20]  = "Position is set";

void FullOpenInit(void)
{
   Clear();
   WriteStaticString(font5x8, 0, 0, Title1);
   WriteStaticString(font5x8, 0, 8, Title2);
   WriteStaticString(font5x8, 0, 16, Title3);
   WriteStaticString(font5x8, 8, 40, Back);
   WriteStaticString(font5x8, 40, 40, Accept);
}

void FullOpenUpdate(void)
{
}

void FullOpenKey(enum PressedButtonState key)
{
   switch (key)
   {
      case PressedButtonDownKey:
         WriteStaticString(font5x8, 0, 30, PositionSet);
         SetFullyOpen();
         TaskSleep(1000);
         SetScreenMode(ModeMainScreenInit);
         break;

      case PressedButtonUpKey:
      case PressedButtonMenuKey:
         SetScreenMode(ModeMainScreenInit);
         break;

      default:
         break;
   }
}
