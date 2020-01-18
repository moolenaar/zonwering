/* lcd.h
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
 * along with zonwering. If not, see <http://ww.gnu.org/licenses/>.
 */ 

#ifndef LCD_H
#define LCD_H

#include "general.h"
#include <stdbool.h>

void LcdSetup(void);
void LcdInitialize(void);
void WriteString(const uint8_t *font, const uint8_t x, const uint8_t y, char* text);
void WriteStaticString(const uint8_t *font, const uint8_t x, const uint8_t y, const char* text);
void Clear(void);
void EnableLcd(bool enable);
void WriteInverted8PixelString(const uint8_t *font, const uint8_t x, uint8_t y, const char* text);
void HandleBacklight(void);
#endif
