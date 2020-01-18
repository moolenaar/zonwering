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

#include <avr/io.h>
#include <stdbool.h>
#include "kernel.h"
#include "motor.h"
#include "clock.h"
#include "nonvolataile.h"

/* variable used to stear the output pins that control the motor */ 
static volatile direction_type motorDirection = DIRECTION_STOP;
/* time value that represents 100% open */
static int16_t fullyOpen;
/* requested time to open */
static int16_t requestedOpenTime = -1;
/* true when the timer to delay closing is active */
static bool timerActive;

direction_type GetMotorDirection(void)
{
   return motorDirection;
}

void MotorOpenPercent(uint8_t value)
{
   requestedOpenTime = ((int32_t)value * (int32_t)fullyOpen) / 100;
}
 
void MotorOpen(void)
{
   StartDown();
   requestedOpenTime = -1;
   motorDirection = DIRECTION_DOWN;
}

void MotorClose(void)
{
   if (GetUpDownTime() > 0)
   {
      StartUp();
      motorDirection = DIRECTION_UP;
   }
   requestedOpenTime = -1;
}

void MotorStop(void)
{
   StopUpDown();
   requestedOpenTime = -1;
   motorDirection = DIRECTION_STOP;
}

uint8_t MotorProgress(void)
{
   uint8_t value = 0;
   if (fullyOpen)
   {
      value = ((int32_t)100 * GetUpDownTime()) / fullyOpen;
   }
   if (value > 100) value = 100;
   return value;
}

void SetFullyOpen(void)
{
   fullyOpen = GetUpDownTime();
}

void MotorSetup(void)
{
   // PA2; drive motor down, PA3: drive motor up
   PORTA &= ~((1 << PORTA2) | (1 << PORTA3)); // make sure outputs are low at startup
   DDRA |= (1 << DDA2) | (1 << DDA3);

   fullyOpen = nvGetFullyOpen();
   timerActive = nvGetTimerActive();
}

void SetMotorOutput(direction_type direction)
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

void MotorDelayClose(uint16_t delayTime)
{
   StartTime(delayTime);
   timerActive = delayTime > 0;   
}

void MotorTask(void)
{
   while (true)
   {
      /* manual moving up but already fully closed */
      if ((motorDirection == DIRECTION_UP) && (GetUpDownTime() == 0))
      {
         /* extra time to make sure fully closed */
         TaskSleep(500);
         StopUpDown();
         motorDirection = DIRECTION_STOP;
      }

      /* start automatic moving down */
      if((motorDirection == DIRECTION_STOP) && (requestedOpenTime > 0) && (requestedOpenTime > GetUpDownTime()))
      {
         StartDown();
         motorDirection = DIRECTION_DOWN;
      }

      /* start automatic moving up */
      if((motorDirection == DIRECTION_STOP) && (requestedOpenTime >= 0) && (requestedOpenTime < GetUpDownTime()))
      {
         StartUp();
         motorDirection = DIRECTION_UP;
      }

      /* stop automatic moving down */
      if ((motorDirection == DIRECTION_DOWN) && (requestedOpenTime > 0) && (requestedOpenTime < GetUpDownTime()))
      {
         StopUpDown();
         requestedOpenTime = -1;
         motorDirection = DIRECTION_STOP;
      }

      /* stop automatic moving up */
      if ((motorDirection == DIRECTION_UP) && (requestedOpenTime > 0) && (requestedOpenTime > GetUpDownTime()))
      {
         StopUpDown();
         requestedOpenTime = -1;
         motorDirection = DIRECTION_STOP;
      }

      /* time delay has passed; close */
      if (timerActive && (GetTime() == 1))
      {
         MotorClose();
         timerActive = false;
      }

      nvSetFullyOpen(fullyOpen);
      nvSetTimerActive(timerActive);
      
      TaskSleep(10);
   }
}
