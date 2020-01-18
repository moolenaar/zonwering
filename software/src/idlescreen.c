/* idlescreen.c
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

static char EEMEM Actual[10] = "Actual";

void idleScreenInit(void)
{
   char buffer[6];

   Clear();
   ProgressBarSetup();
   ProgressBar(MotorProgress());
   WriteStaticString(font5x8, 40, 16, Actual);
   WriteString(font6x10, 50, 26, utoaRightAligned(MotorProgress(), buffer));

   EnableLcd(false);
}

void idleScreenKey(enum PressedButtonState key)
{
   switch (key)
   {
      case PressedButtonDownKey:
      case PressedButtonUpKey:
      case PressedButtonMenuKey:
         EnableLcd(true);
         SetScreenMode(ModeMainScreenInit);
         break;

      default:
         break;
   }
}

void idleScreenUpdate(void)
{
  /* nothing to do here */
}
