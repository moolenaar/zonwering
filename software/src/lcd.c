/* lcd.c
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

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "kernel.h"
#include "lcd.h"

uint8_t active;

void EnableLcd(bool enable)
{
   if (enable)
   {
      PORTB &= ~(1 << PORTB0);
      active = 10;
   }
   else
   {
      // enableing the LCD also turns on the backlight
      PORTB |= (1 << PORTB0);
   }
}

void LcdSetup(void)
{
   PORTB &= ~((1 << PORTB1) | (1 << PORTB2));
   PORTB |= (1 << PORTB0);
   DDRB |= (1 << DDB0)      // PB0; chip enable active low
        |  (1 << DDB1)      // PB1: data register (high) / command register (low)
        |  (1 << DDB2);     // PB2: reset active low

   USICR = (1 << USICS1) | (1 << USICLK);
   DDRA |= (1 << PORTA4)    // uclk as output
        |  (1 << PORTA5);   // LCD Din as output

   PORTB |= (1 << PORTB2);  // nRESET high

   EnableLcd(true);
}

static void WriteLcdData(uint8_t data)
{
   USIDR = data;
   USISR = (1 << USIOIF);                // clear counter and counter overflow interrupt flag
   while (!(USISR & (1 << USIOIF)) )
   {
      USICR = (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 << USITC);
   }
}

static void WriteLcdCommand(uint8_t data)
{
   PORTB &= ~(1 << DDB1);  // PB1: select command register (low)
   WriteLcdData(data);
   PORTB |= (1 << DDB1);   // PB1: select data register (high)
}

static void SetAddress(uint8_t x, uint8_t y)
{
   WriteLcdCommand(0x40 + y); // 0 <= y <= 5; bytes
   WriteLcdCommand(0x80 + x); // 0 <= x <= 83; pixels
}

void Clear(void)
{
   WriteLcdCommand(8);    // display control; switch display off

   SetAddress(0, 0);
   for (uint16_t i = 0; i < 84 * 48 / 8; ++i)
   {
      WriteLcdData(0);
   }
   WriteLcdCommand(0x0C); // display control; switch display on
}

void LcdInitialize(void)
{
   WriteLcdCommand(0x21); // function set; H = 1
   WriteLcdCommand(0x14); // bias value 4 (1 : 48)
   WriteLcdCommand(0xb2); // set Vop to 3 + 50 * 0.06 = 6V
   WriteLcdCommand(0x06); // temperature control
   WriteLcdCommand(0x20); // function set; H = 0, horizontal addressing mode
   Clear();               // clear ram
}

static inline uint8_t GetNrRows(const uint8_t *font)
{
   return pgm_read_byte_near(&(font[1]));
}

static inline uint16_t FindCharacter(const uint8_t *font, uint8_t value)
{
   uint8_t nrCharacters = pgm_read_byte_near(&(font[0]));
   uint16_t index = 2;

   for (uint8_t i = 0; i < nrCharacters; ++i)
   {
      if (value == pgm_read_byte_near(&(font[index])))
      {
         break;
      }
      else
      {
         index += pgm_read_byte_near(&(font[index + 1])) + 2;
      }
   }
   return index + 2;
}

static inline uint8_t GetNrColumns(const uint8_t *font, uint16_t characterIndex)
{
   return pgm_read_byte_near(&(font[characterIndex - 1]));
}

static inline uint8_t GetColumnData(const uint8_t *font, uint16_t characterIndex, uint8_t position)
{
   return pgm_read_byte_near(&(font[characterIndex + position]));
}

static char ReadStringFromEeprom(const char* address)
{
   return eeprom_read_byte((uint8_t *)address);
}

static char ReadStringFromRam(const char* address)
{
   return *address;
}

static inline void Write8PixelString(const uint8_t *font, const uint8_t x, uint8_t y, char (*source)(const char*), const char* text)
{
   uint8_t data, value, ch, index = 0;

   SetAddress(x, y / 8);

   ch = source(&text[index++]);
   while ((ch != 0) && (ch != 255))
   {
      uint16_t characterIndex = FindCharacter(font, ch);
      uint8_t nrColumns = GetNrColumns(font, characterIndex);
      ch = source(&text[index++]);
      for (uint8_t n = 0; n < nrColumns; ++n)
      {
         value = GetColumnData(font, characterIndex, n);
         data = value << (y % 8);
         WriteLcdData(data);
      }
      TaskSleep(0);
   }

   if (y % 8 > 0)
   {
      index = 0;
      ch = source(&text[index++]);
      SetAddress(x, y / 8 + 1);
      while (ch != 0)
      {
         uint16_t characterIndex = FindCharacter(font, ch);
         uint8_t nrColumns = GetNrColumns(font, characterIndex);
         ch = source(&text[index++]);
         for (uint8_t n = 0; n < nrColumns; ++n)
         {
            data = GetColumnData(font, characterIndex, n);
            data >>= (8 - (y % 8));
            WriteLcdData(data);
         }
         TaskSleep(0);
      }
   }
   SetAddress(x, y / 8);
}

void static inline Write16PixelString(const uint8_t *font, const uint8_t x, uint8_t y, char (*source)(const char*), const char* text)
{
   uint8_t data, value, ch, index = 0;

   SetAddress(x, y / 8);

   ch = source(&text[index++]);
   while (ch != 0)
   {
      uint16_t characterIndex = FindCharacter(font, ch);
      uint8_t nrColumns = GetNrColumns(font, characterIndex) / 2;
      ch = source(&text[index++]);
      for (uint8_t n = 0; n < nrColumns; ++n)
      {
         value = GetColumnData(font, characterIndex, n * 2);
         data = value << (y % 8);
         WriteLcdData(data);
      }
      TaskSleep(0);
   }

   SetAddress(x, y / 8 + 1);
   index = 0;
   ch = source(&text[index++]);
   while (ch != 0)
   {
      uint16_t characterIndex = FindCharacter(font, ch);
      uint8_t nrColumns = GetNrColumns(font, characterIndex) / 2;
      ch = source(&text[index++]);
      for (uint8_t n = 0; n < nrColumns; ++n)
      {
         value = GetColumnData(font, characterIndex, n * 2);
         data = value >> (8 - (y % 8));
         value = GetColumnData(font, characterIndex, n * 2 + 1);
         data |= value << (y % 8);
         WriteLcdData(data);
      }
      TaskSleep(0);
   }
   SetAddress(x, y / 8);
}

void WriteString(const uint8_t *font, const uint8_t x, const uint8_t y, char* text)
{
   if (GetNrRows(font) > 8)
   {
      Write16PixelString(font, x, y, ReadStringFromRam, text);
   }
   else
   {
      Write8PixelString(font, x, y, ReadStringFromRam, text);
   }
}

void WriteStaticString(const uint8_t *font, uint8_t x, uint8_t y, const char* text)
{
   if (GetNrRows(font) > 8)
   {
      Write16PixelString(font, x, y, ReadStringFromEeprom, text);
   }
   else
   {
      Write8PixelString(font, x, y, ReadStringFromEeprom, text);
   }
}

void WriteInverted8PixelString(const uint8_t *font, const uint8_t x, uint8_t y, const char* text)
{
   uint8_t data, value, ch, index = 0;

   SetAddress(x, y / 8);

   ch = eeprom_read_byte((const uint8_t *)&text[index++]);
   while (ch != 0)
   {
      uint16_t characterIndex = FindCharacter(font, ch);
      uint8_t nrColumns = GetNrColumns(font, characterIndex);
      ch = eeprom_read_byte((const uint8_t *)&text[index++]);
      for (uint8_t n = 0; n < nrColumns; ++n)
      {
         value = GetColumnData(font, characterIndex, n);
         data = value << (y % 8);
         WriteLcdData(~data);
      }
      TaskSleep(0);
   }

   if (y % 8 > 0)
   {
      index = 0;
      ch = eeprom_read_byte((const uint8_t *)&text[index++]);
      SetAddress(x, y / 8 + 1);
      while (ch != 0)
      {
         uint16_t characterIndex = FindCharacter(font, ch);
         uint8_t nrColumns = GetNrColumns(font, characterIndex);
         ch = eeprom_read_byte((const uint8_t *)&text[index++]);
         for (uint8_t n = 0; n < nrColumns; ++n)
         {
            data = GetColumnData(font, characterIndex, n);
            data >>= (8 - (y % 8));
            WriteLcdData(~data);
         }
         TaskSleep(0);
      }
   }
   SetAddress(x, y / 8);
}
