/* motor.h
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

#ifndef MOTOR_H
#define MOTOR_H

#include "general.h"

typedef enum
{
   DIRECTION_STOP,
   DIRECTION_UP,
   DIRECTION_DOWN
} direction_type;

void SetMotorOutput(direction_type direction);
direction_type GetMotorDirection(void);
void MotorOpenPercent(uint8_t value);
void MotorOpen(void);
void MotorClose(void);
void MotorStop(void);
uint8_t MotorProgress(void);
void MotorSetup(void);
void MotorTask(void);
void SetFullyOpen(void);
void MotorDelayClose(uint16_t delayTime);

#endif
