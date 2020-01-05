/* adc.h
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

#ifndef ADC_H
#define ADC_H

#include "general.h"

enum ButtonAdc
{
   ButtonAdcNone = 0,
   ButtonAdcDown,
   ButtonAdcUp,
   ButtonAdcMenu
};

extern void AdcSetup(void);
extern void AdcTask(void);
extern int16_t GetVoltage(void);
extern enum ButtonAdc GetAdcButton(void);
extern uint16_t GetNrSamples(void);

#endif
