/*
 * nonvolataile.c
 *
 * Copyright 2019 Herman Moolenaar
 * 
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

#include "nonvolataile.h"
#include <avr/eeprom.h>
#include <stdbool.h>
#include "kernel.h"
#include "lcd.h"

static volatile uint32_t timeCounter;
static volatile uint16_t upDownCounter;
static int16_t fullyOpen;
static bool timerActive; 

void NonVolataileSetup(void)
{
   timeCounter = 0;//eeprom_read_dword((uint32_t*)500);
   upDownCounter = 0;//eeprom_read_word((uint16_t*)504);
   fullyOpen = 1000;//(int16_t)eeprom_read_word((uint16_t*)506);
   timerActive = false;//eeprom_read_byte((uint8_t*)508);
}

uint32_t nvGetTimeCounter(void)
{
   return timeCounter;
}

void nvSetTimeCounter(uint32_t value)
{
   timeCounter = value;
}

uint16_t nvGetUpDownCounter(void)
{
   return upDownCounter;
}

void nvSetUpDownCounter(uint16_t  value)
{
   upDownCounter = value;
}

int16_t nvGetFullyOpen(void)
{
   return fullyOpen;
}

void nvSetFullyOpen(int16_t value)
{
   fullyOpen = value;
}

bool nvGetTimerActive(void)
{
   return timerActive;
}

void nvSetTimerActive(bool value)
{
   timerActive = value;
}

void NonVolataileTask(void)
{
   while(true)
   {
      eeprom_update_dword((uint32_t*)500, timeCounter);
      TaskSleep(0);
      eeprom_update_word((uint16_t*)504, upDownCounter);
      TaskSleep(0);
      eeprom_update_word((uint16_t*)506, (uint16_t)fullyOpen);
      TaskSleep(0);
      eeprom_update_byte((uint8_t*)508, timerActive);
      TaskSleep(0);
      HandleBacklight();
      TaskSleep(500);
   }
}

