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
#include "digitaloutput.h"
#include "display.h"
#include "adc.h"
#include "motor.h"
#include "button.h"
#include "lcd.h"

//#define test

#define DISPLAYSTACKSIZE MINIMUMSTACKSIZE + 40
uint8_t displayStack[DISPLAYSTACKSIZE];

#define MOTORSTACKSIZE MINIMUMSTACKSIZE + 2
uint8_t motorStack[MOTORSTACKSIZE];

#define BUTTONSTACKSIZE MINIMUMSTACKSIZE + 2
uint8_t buttonStack[BUTTONSTACKSIZE];

#define DIGITALOUTPUTSTACKSIZE MINIMUMSTACKSIZE + 12
uint8_t digitalOutputStack[DIGITALOUTPUTSTACKSIZE];

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
   KernelSetup();
   DigitalOutputSetup();
   DisplaySetup();
   AdcSetup();
   MotorSetup();

#ifdef test
   InitTask(TESTSTACKSIZE, testStack, TestTask);
#else
   InitTask(DISPLAYSTACKSIZE, displayStack, DisplayTask);
   InitTask(MOTORSTACKSIZE, motorStack, MotorTask);
   InitTask(BUTTONSTACKSIZE, buttonStack, ButtonTask);
   InitTask(DIGITALOUTPUTSTACKSIZE, digitalOutputStack, DigitalOutputTask);
#endif

   StartKernel(NULL);
}

