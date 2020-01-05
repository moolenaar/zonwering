/* digitaloutput.c
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

#include "digitaloutput.h"
#include <avr/io.h>
#include "button.h"

void DigitalOutputSetup(void)
{
   // PA2; drive motor down, PA3: motor up
   PORTA &= ~((1 << PORTA2) | (1 << PORTA3)); // make sure outputs are low at startup
   DDRA |= (1 << DDA2) | (1 << DDA3);
}

void MotorDirection(direction_type direction)
{
   switch (direction)
   {
      case DIRECTION_UP:
         PORTA &= ~(1 << PORTA2);
         PORTA |= (1 << PORTA3);
         break;
      case DIRECTION_DOWN:
         PORTA &= ~(1 << PORTA3);
         PORTA |= (1 << PORTA2);
         break;
      case DIRECTION_STOP:
         PORTA &= ~((1 << PORTA2) | (1 << PORTA3));
         break;
   }
}

void DigitalOutputTask(void)
{
   enum PressedButtonState buttonState;

   while (true)
   {
      // buttonState = GetButtonState();
      // if (buttonState == PressedButtonDown)
      // {
      //    MotorDirection(DIRECTION_DOWN);
      // }
      // else if (buttonState == PressedButtonUp)
      // {
      //    MotorDirection(DIRECTION_UP);
      // }
      // else if (buttonState == PressedButtonNone)
      // {
      //    MotorDirection(DIRECTION_STOP);
      // }

      TaskSleep(100);
   }
}