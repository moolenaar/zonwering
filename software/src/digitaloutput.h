/* digitaloutput.h
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

#ifndef DIGITALOUTPUT_H
#define DIGITALOUTPUT_H

#include "general.h"

typedef enum
{
   DIRECTION_STOP,
   DIRECTION_UP,
   DIRECTION_DOWN
} direction_type;

extern void MotorDirection(direction_type direction);
extern void DigitalOutputSetup(void);
extern void DigitalOutputTask(void);

#endif
