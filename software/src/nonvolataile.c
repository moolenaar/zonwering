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

static uint16_t delaySeconds;
 
void NonVolataileSetup(void)
{
   delaySeconds = eeprom_read_word((uint16_t*) 64);
}

uint16_t NonVolataileRead(void)
{
   return delaySeconds;
}

void NonVolataileWrite(uint16_t value)
{
   if (value != delaySeconds)
   {
      delaySeconds = value;
      eeprom_write_word((uint16_t*) 64, delaySeconds);
   }
}

uint8_t config_ee[15] EEMEM = "testing ...";
