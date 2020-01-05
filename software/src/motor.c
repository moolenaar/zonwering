/* motor.c
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

#include "kernel.h"
#include "motor.h"
#include "button.h"
#include "digitaloutput.h"

static direction_type motorDirection = DIRECTION_STOP;

direction_type GetMotorDirection(void)
{
   return motorDirection;
}

void SetMotorDirection(direction_type direction)
{
   motorDirection = direction;
}

void MotorSetup(void)
{

}

void MotorTask(void)
{
   while (true)
   {
//      enum ButtonState button = GetButton();
//
//      if ((button == ButtonDown) || (button == ButtonDownRepeat))
//      {
//         motorDirection = DIRECTION_DOWN;
//      }
//      else if ((button == ButtonUp) || (button == ButtonUpRepeat))
//      {
//         motorDirection = DIRECTION_UP;
//      }
//      else
//      {
//         motorDirection = DIRECTION_STOP;
//      }
      TaskSleep(10);
   }
}
