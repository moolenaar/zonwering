/* display.h
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "general.h"

enum ScreenModeType
{
   ModeProductTitle = 0,
   ModeMainScreenInit,
   ModeMainScreenUpdate,
   ModeAskClosingTimeInit,
   ModeAskClosingTimeUpdate,
   ModeAskFullOpenInit,
   ModeAskFullOpenUpdate,
   ModeDiagnosticInit,
   ModeDiagnosticUpdate,
   ModeIdleInit,
   ModeIdleUpdate
};

void HandleDisplay(void);
enum ScreenModeType GetScreenMode(void);
void SetScreenMode(enum ScreenModeType screenMode);
void ProgressBarSetup(void);
void ProgressBar(uint8_t targetValue);
char *UtoaRightAligned(uint8_t value, char *buffer);
void ResetBacklightTimer(void);

#endif
