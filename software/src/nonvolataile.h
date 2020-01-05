/*
 * nonvolataile.h
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

#ifndef NONVOLATAILE_H_
#define NONVOLATAILE_H_

#include "general.h"

/* setup the software that controls reading and writing of the eeprom
 */
extern void NonVolataileSetup(void);

/* read an uint16 from eeprom
 * returns the value read
 */
extern uint16_t NonVolataileRead(void);

/* write an uint16 to eeprom
 * value: the value to write to eeprom
 */
extern void NonVolataileWrite(uint16_t value);

#endif /* NONVOLATAILE_H_ */
