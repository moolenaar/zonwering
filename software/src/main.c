/* main.c
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

#include <stdlib.h>
#include <general.h>
#include "kernel.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "lcd.h"
#include "nonvolataile.h"
#include "clock.h"

//#define test

#define MOTORSTACKSIZE MINIMUMSTACKSIZE + 22
uint8_t motorStack[MOTORSTACKSIZE];

#define BUTTONSTACKSIZE MINIMUMSTACKSIZE + 60
uint8_t buttonStack[BUTTONSTACKSIZE];

#define EEPROMTASKSTACKSIZE MINIMUMSTACKSIZE + 12
uint8_t eepromTaskStack[EEPROMTASKSTACKSIZE];

#ifdef test

#define TESTSTACKSIZE MINIMUMSTACKSIZE + 40
uint8_t testStack[TESTSTACKSIZE];

void TestTask(void)
{
   while(true)
   {
      MotorDirection(DIRECTION_UP);
      EnableLcd(false);
      TaskSleep(500);
      MotorDirection(DIRECTION_DOWN);
      EnableLcd(true);
      TaskSleep(500);
   }
}
#endif

int main (void)
{
   NonVolataileSetup();
   KernelSetup();
   AdcSetup();
   MotorSetup();
   ButtonSetup();
   ClockInit();

#ifdef test
   InitTask(TESTSTACKSIZE, testStack, TestTask);
#else
   InitTask(EEPROMTASKSTACKSIZE, eepromTaskStack, NonVolataileTask);
   InitTask(MOTORSTACKSIZE, motorStack, MotorTask);
   InitTask(BUTTONSTACKSIZE, buttonStack, ButtonTask);
#endif

   StartKernel(NULL);
}

