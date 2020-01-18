/*
 * button.c
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

#include "kernel.h"
#include "button.h"
#include "adc.h"
#include "lcd.h"
#include "display.h"

#define LONGPRESS 100

static enum PressedButtonState pressedButton = PressedButtonNone;
static void (*keyHandler)(enum PressedButtonState);

static inline enum PressedButtonState AdcButtonToPressedButton(enum ButtonAdc adcButton)
{
   switch (adcButton)
   {
      case ButtonAdcDown: return PressedButtonDown;
      case ButtonAdcUp:   return PressedButtonUp;
      case ButtonAdcMenu: return PressedButtonMenu;
      case ButtonAdcNone:
      default:
         return PressedButtonNone;
   }
}

static inline void CheckButtonPress(void)
{
   static enum ButtonAdc oldAdcButton = ButtonAdcNone;
   static uint8_t count = 0;
   enum ButtonAdc newAdcButton = GetAdcButton();

   if (oldAdcButton != newAdcButton)
   {
      count = 0;
      if (keyHandler)
      {
         if (pressedButton % 3 == 1)
         {
            keyHandler(PressedButtonNone);
         }
         else
         {
            keyHandler(pressedButton + 2);
         }
      }
      pressedButton = PressedButtonNone;
      oldAdcButton = newAdcButton;
   }

   if (newAdcButton != ButtonAdcNone)
   {
      count++;

      if (count == 5)
      {
         ResetBacklightTimer();
         pressedButton = AdcButtonToPressedButton(newAdcButton);
         if (keyHandler) keyHandler(pressedButton);
      }
      else if (count == LONGPRESS)
      {
         pressedButton++;
         if (keyHandler) keyHandler(pressedButton);
      }
      else if (count > LONGPRESS)
      {
        count = LONGPRESS + 1;
      }
   }
}

void SetKeyHandler(void (*handler)(enum PressedButtonState))
{
   keyHandler = handler;
}

void ButtonSetup(void)
{
   LcdSetup();
   LcdInitialize();
}

void ButtonTask(void)
{
   while (true)
   {
      CheckButtonPress();
      TaskSleep(5);
      HandleDisplay();
      TaskSleep(5);
   }
}
