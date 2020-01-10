/* clock.c
 * Copyright 2019 Herman Moolenaar
 * This file is part of LCR-Meter.
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
#include <stdbool.h>

static volatile uint32_t timeCounter = 0;
static volatile uint16_t upDownCounter = 0;
static volatile int8_t step = 0;

void StartTime(void)
{
   timeCounter = 0;
}

uint16_t GetTime(void)
{
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
   return upDownCounter;
}

void HandleClock(void)
{
   upDownCounter += step;
   if (upDownCounter == 0) step = 0;
   timeCounter++;
}
