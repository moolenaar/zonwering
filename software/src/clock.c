/* clock.c
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


#include "clock.h"
#include "nonvolataile.h"

static volatile uint32_t timeCounter;
static volatile uint16_t upDownCounter;
static volatile int8_t step = 0;

void ClockInit(void)
{
   timeCounter = nvGetTimeCounter();
   upDownCounter = nvGetUpDownCounter();
}

void StartTime(uint16_t waitTime)
{
   timeCounter = (uint32_t)waitTime * 50 * 60;
}

uint16_t GetTime(void)
{
   /* minutes resolution */
   return timeCounter / 50 / 60;
}

void StartDown(void)
{
   step = 1;
}

void StartUp(void)
{
   step = -1;
}

void StopUpDown(void)
{
   step = 0;
}

void ResetUpDown(void)
{
   step = 0;
   upDownCounter = 0;
}

uint16_t GetUpDownTime(void)
{
   /* 100ms resolution */
   return upDownCounter / 5;
}

void HandleClock(void)
{
   upDownCounter += step;
   if ((upDownCounter == 0) || (upDownCounter == 0xFFFF)) step = 0;
   if (timeCounter > 0) timeCounter--;

   nvSetUpDownCounter(upDownCounter);
   nvSetTimeCounter(timeCounter);
}

