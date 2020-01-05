#!/usr/bin/python
# img2c.py - convert a bitmap files into a c source file for inclusion
# into the project
# Copyright 2014 - 2019 Herman Moolenaar 
# This file is part of zonwering.
#
# Zonwering is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Zonwering is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with zonwering.  If not, see <http://www.gnu.org/licenses/>.

import sys
import os
from PIL import Image
from os.path import basename
from optparse import OptionParser
from Crypto.Util.number import size

imageWithPalette = Image.new("P", (1,1))
imageWithPalette.putpalette((255,255,255, 0,0,0) + (0,0,0) * 254)

def checkRequiredArguments(opts, parser):
    missing_options = []
    for option in parser.option_list:
        if option.help.startswith('[REQUIRED]') and eval('opts.' + option.dest) == None:
            missing_options.extend(option._long_opts)
    if len(missing_options) > 0:
        parser.error('Missing REQUIRED parameters: ' + str(missing_options))
        exit(-1)

def WriteStartSourceFile(f, fileName):
   f.write('/* Generated file, do not edit */\n\n') 
   f.write('#include "' + fileName + '.h"\n\n') 
   f.write('#include <avr/pgmspace.h>\n\n') 
   
def WriteStartHeaderFile(f, fileName):
   f.write('/* Generated file, do not edit */\n\n') 
   f.write('#ifndef ' + fileName.upper() + '_H' + '\n') 
   f.write('#define ' + fileName.upper() + '_H' + '\n\n') 
   f.write('#include <stdint.h>\n\n') 

def WriteEndHeaderFile(f):
   f.write('#endif\n\n')

def WriteEndSourceFile(f):
   f.write('\n')

def WriteStartFontData(sourceFile, fontName, height, nrCharacters, arraySize):
   sourceFile.write('const uint8_t {}[{}] PROGMEM =\n'.format(fontName, arraySize))
   sourceFile.write('{{\n   {}, {},\n'.format(hex(nrCharacters), hex(height)))

def WriteFontData(f, fontData):
   f.write('   ')
   for ch in fontData:
      f.write('{}, '.format(hex(ch)))
   f.write('\n')

def WriteEndFontData(sourceFile):
   sourceFile.write('\n};\n\n')

def WriteFontToHeaderFile(headerFile, fontName, arraySize):
   headerFile.write('extern const uint8_t {}[{}];\n\n'.format(fontName, arraySize))

def FindFiles(sourceDirectory, fontName):
   id = []
   for f in os.listdir(sourceDirectory):
      if ((f.endswith(".png")) & (f.startswith(fontName))):
          baseName, extension = os.path.splitext(f)
          id.append(int(baseName[baseName.find('_') + 1 :]))
   id.sort()
   imgFiles = ["{}_{}.png".format(fontName, f) for f in id]
   return imgFiles

def FontCharacteristics(imgFiles):
   numberOfBytesPerColumn = 0
   totalNumberOfColumns = 0
   numberOfCharacters = len(imgFiles)
   for imgFileName in imgFiles:
      sourceImage = Image.open(os.path.join(options.imgDir, imgFileName))
      width, height = sourceImage.size
      
      heightInBytes = height / 8
      if (height % 8 != 0):
         heightInBytes += 1
      if (numberOfBytesPerColumn < heightInBytes):
         numberOfBytesPerColumn = heightInBytes
      totalNumberOfColumns += width
         
   return height, numberOfBytesPerColumn, totalNumberOfColumns, numberOfCharacters

def AsciiCodeFont(imgFileName):
   baseName, extension = os.path.splitext(imgFileName)
   fontCode = int(baseName[baseName.find('_') + 1 :])
   return fontCode

def FontSizeInBytes(height, width):
   imageSizeBytes = height / 8
   if (height % 8 != 0):
      imageSizeBytes += 1
   imageSizeBytes *= width
   return imageSizeBytes

def FontToData(imgFileName):
   sourceImage = Image.open(imgFileName).convert("RGB")
   newImage = sourceImage.quantize(colors=2, method=2, kmeans=0, palette=imageWithPalette)
   # newImage orientation: first top horizontal row, then the row below the top horizontal row, etc.
   imageData = list(newImage.getdata())
   width, height = newImage.size
   
   # the font bitmap data
   fontData = []
   fontData.append(AsciiCodeFont(imgFileName))
   fontData.append(FontSizeInBytes(height, width))

   value = 0
   for w in range(width):
      for h in range(height):
         value |= imageData[w + h * width] << (h % 8)
         if (h % 8 == 7):
            fontData.append(value)
            value = 0
      if (height % 8 != 0):
         fontData.append(value)
         value = 0

   return fontData

# handle command line options
parser = OptionParser(usage="%prog [-d] [-o] [-f]", version="%prog 1.0")
parser.add_option("-d", "--directory", help="[REQUIRED] directory containing the image files", dest="imgDir")
parser.add_option("-o", "--outputfile", help="[REQUIRED] destination filename (without extension) for writing the image code to", dest="outfile")
parser.add_option("-f", "--fontname", help="[REQUIRED] name(s) of one or more fonts to process", action="append", dest="fontname")

(options, args) = parser.parse_args()
checkRequiredArguments(options, parser)

fullPath, nameAndExtension = os.path.split(options.outfile)
outputName, extension = os.path.splitext(nameAndExtension)

sourceFile = open(os.path.join(fullPath, outputName + '.c'), 'w')
headerFile = open(os.path.join(fullPath, outputName + '.h'), 'w')

WriteStartSourceFile(sourceFile, outputName)
WriteStartHeaderFile(headerFile, outputName)

sourceDirectory = options.imgDir

for fontName in options.fontname:
   imgFiles = FindFiles(sourceDirectory, fontName)
   height, bytesPerRow, totalNumberOfColumns, numberOfCharacters = FontCharacteristics(imgFiles)
   totalSize = (totalNumberOfColumns * bytesPerRow) + len(imgFiles) * 2 + 2
   WriteFontToHeaderFile(headerFile, fontName, totalSize)
   WriteStartFontData(sourceFile, fontName, height, len(imgFiles), totalSize)

   for imgFileName in imgFiles:
      fontData = FontToData(os.path.join(options.imgDir, imgFileName))
      WriteFontData(sourceFile, fontData)
      
   WriteEndFontData(sourceFile)

sourceFile.close()
WriteEndHeaderFile(headerFile)
headerFile.close()

exit(0)
