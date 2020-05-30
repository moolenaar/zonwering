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

#include "nonvolatile.h"
#include <avr/eeprom.h>
#include <stdbool.h>
#include <string.h>
#include "kernel.h"
#include "lcd.h"

typedef struct
{
   uint32_t timeCounter;
   uint16_t upDownCounter;
   int16_t fullyOpen;
   bool timerActive;
   uint8_t step;
   uint8_t openPercent;
   uint8_t checksum;
} DataType;

typedef union 
{
   DataType data;
   uint8_t buffer[20];
} DataUnion; 

static DataUnion EEMEM eeprom =
{ 
   .buffer = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },   
   .data = { 0, 0, 1000, false, 30, 0, 0xf7 }
};

static DataUnion nv;

static uint8_t calculateChecksum(void)
{
   uint8_t i;
   uint8_t checksum = 0;

   for (i = 0; i < sizeof(DataUnion); i++)
   {
      checksum += nv.buffer[i];
   }
   return checksum;
}

void NonVolataileSetup(void)
{
   eeprom_read_block(&nv.buffer, eeprom.buffer, sizeof(DataUnion));
   if(calculateChecksum() != 0)
   {
      memset(nv.buffer, 0, sizeof(DataUnion));
   }
}

uint32_t nvGetTimeCounter(void)
{
   return nv.data.timeCounter;
}

void nvSetTimeCounter(uint32_t value)
{
   nv.data.timeCounter = value;
}

uint16_t nvGetUpDownCounter(void)
{
   return nv.data.upDownCounter;
}

void nvSetUpDownCounter(uint16_t  value)
{
   nv.data.upDownCounter = value;
}

int16_t nvGetFullyOpen(void)
{
   return nv.data.fullyOpen;
}

void nvSetFullyOpen(int16_t value)
{
   nv.data.fullyOpen = value;
}

bool nvGetTimerActive(void)
{
   return nv.data.timerActive;
}

void nvSetTimerActive(bool value)
{
   nv.data.timerActive = value;
}

uint8_t nvGetOpenPercent(void)
{
   return nv.data.openPercent;
}

void nvSetOpenPercent(uint8_t value)
{
   nv.data.openPercent = value;
}

void NonVolataileTask(void)
{
   while(true)
   {
      nv.data.checksum = 0;
      nv.data.checksum = ~calculateChecksum() + 1;
      eeprom_update_block(&nv.buffer, eeprom.buffer, sizeof(DataUnion));
      TaskSleep(500);
   }
}

