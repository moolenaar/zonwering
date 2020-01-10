/* adc.c
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

#include <avr/interrupt.h>
#include <util/atomic.h>
#include "adc.h"
#include "kernel.h"
#include "stdlib.h"
#include "motor.h"
#include "clock.h"

#define COUNTER_TIME_50HZ (2500 - 1)
#define COUNTER_DELAY_BUTTON (1000 - 1)
#define COUNTER_DELTA 200

static volatile int16_t button = 0;

int16_t GetVoltage(void)
{
   int16_t value;
   ATOMIC_BLOCK(ATOMIC_FORCEON)
   {
      value = button; //voltage;
   }
   return value * 4 / 3;
}

enum ButtonAdc GetAdcButton(void)
{
   enum ButtonAdc value;

   ATOMIC_BLOCK(ATOMIC_FORCEON)
   {
      if ((button >= 480) && (button <= 520))        value = ButtonAdcMenu;  // 1.60V, cnt=500
      else if ((button >= 658) && (button <= 698))   value = ButtonAdcDown;  // 1.05V, cnt=680
      else if ((button >= 1000) && (button <= 1040)) value = ButtonAdcUp;    // 3.30V, cnt=1023
      else value = ButtonAdcNone;
   }
   return value;
}

uint16_t GetNrSamples(void)
{
   uint16_t value;
   ATOMIC_BLOCK(ATOMIC_FORCEON)
   {
      value = OCR1A;
   }
   return value;
}

void AdcSetup(void)
{
   /* ADC setup */
   DIDR0 = (1 << ADC0D) | (1 << ADC1D);                     // disable digital buffer for ADC0 and ADC1
   ADMUX = 0;                                               // next digitize buttons and 5V reference

   ADCSRB = (1 << ADTS2) | (1 << ADTS0);                    // start conversion on timer 1 compare B
   ADCSRA = (1 << ADEN) |                                   // enable ADC
            (1 << ADIF) |                                   // disable any interrupts
            (1 << ADSC) |
            (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0) |    // 125KHz ADC clock
            (1 << ADIE) |                                   // generate interrupt when ready
            (1 << ADATE);                                   // use interrupt source to start ADC

   /* timer 1 setup */
   TCCR1A = 0;                         // CTC mode
   OCR1A = COUNTER_TIME_50HZ;          // 2500 - 1 gives 20 ms delay time
   OCR1B = COUNTER_DELAY_BUTTON;       // OCR1B is used to trigger the ADC. The ADC is used for buttons and zero detection
   TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode and 8 MHz divide by 64 prescaler produces 125 KHz clock source
}

ISR (ADC_vect)
{
   static uint16_t average = COUNTER_TIME_50HZ;

   if (OCR1B == COUNTER_DELAY_BUTTON)
   {
      // buttons
      button = ADC;

      // next adc measurement will be zero crossing
      ADMUX = 1;
      OCR1B = 0;
      SetMotorOutput(GetMotorDirection());
   }
   else
   {
      // zero crossing
      OCR1A = average + (465 - ADC);
      average = (average * 9 + OCR1A) / 10;
      if (average > COUNTER_TIME_50HZ + COUNTER_DELTA) average = COUNTER_TIME_50HZ + COUNTER_DELTA;
      if (average < COUNTER_TIME_50HZ - COUNTER_DELTA) average = COUNTER_TIME_50HZ - COUNTER_DELTA;

      // next adc measurement will be buttons
      ADMUX = 0;
      OCR1B = COUNTER_DELAY_BUTTON;
   }
   TIFR1 = (1 << TOV1) | (1 << OCF1B) | (1 << OCF1A) | (1 << ICF1);

   HandleClock();
}
