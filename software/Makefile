# install: gcc-avr, avr-libc and python Pillow
MCU_TARGET        = attiny84
AVRDUDETARGET     = t84
TARGET            = zonwering
SRC               = main.c kernel.c font.c adc.c lcd.c button.c display.c motor.c idlescreen.c \
                    mainscreen.c fullopen.c closingtimescreen.c clock.c nonvolatile.c
OPTIMIZE          = -Os
LIBS              = 
INCFLAGS          =
INCLUDE           = -I src -I /usr/lib/avr/include
EXTRA_CLEAN_FILES = *.hex *.bin *.srec
CC                = avr-gcc
CFLAGS            = -g -Wall -Wstrict-prototypes $(OPTIMIZE) -std=c99 -mmcu=$(MCU_TARGET) -D __AVR_attiny84a__ -fshort-enums
LFUSE             = 0xE2
HFUSE             = 0xD7
EFUSE             = 0xFF
LDFLAGS           = 
OBJCOPY           = avr-objcopy
OBJDUMP           = avr-objdump
SIZE              = avr-size

ifndef VERBOSE
#.SILENT:
endif

all: build build/$(TARGET).hex build/$(TARGET)_eeprom.hex build/$(TARGET).lss build/$(TARGET).sym size

OBJ = $(patsubst %.c,build/%.o,$(SRC))

LST = $(ASRC:.S=.lst)

# create build output directory
build:
	mkdir -p build

# compile
build/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# link
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
build/$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

# create intel hex file to be programmed into flash memory
build/$(TARGET).hex: build/$(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# create intel hex file to be programmed into eeprom memory
build/$(TARGET)_eeprom.hex: build/$(TARGET).elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

# print rom size
size:
	if [ -f build/$(TARGET).elf ]; then \
		$(SIZE) --format=avr --mcu=$(MCU_TARGET) build/$(TARGET).elf; \
	fi

# Create extended listing file from ELF output file.
build/%.lss: build/%.elf
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
build/%.sym: build/%.elf
	avr-nm -n $< > $@

# program the program into the chip
install: build/$(TARGET).hex build/$(TARGET)_eeprom.hex
	avrdude -c avrisp2 -p $(AVRDUDETARGET) -Pusb -B 4 -Uflash:w:build/$(TARGET).hex -Ueeprom:w:build/$(TARGET)_eeprom.hex

fuse:
	avrdude -c avrisp2 -p $(AVRDUDETARGET) -Pusb -B 1024 -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

clean:
	rm -rf build
	rm src/font.*

# source code dependencies
build/%.d: src/%.c
	if [ -d "build" ]; then \
		set -e; $(CC) -MM $(CFLAGS) $(INCLUDE) $< \
		| sed 's,\(.*\)\.o[ :]*,\1.o \1.d : ,g' > $@; \
		[ -s $@ ] || rm -f $@; \
	fi

# fonts
src/font.c: img2c/fonts/font5x8_*.png img2c/fonts/lines5x12_*.png img2c/fonts/font6x10_*.png
	./img2c/img2c.py -d img2c/fonts -o src/font -f font5x8 -f lines5x12 -f font6x10

# Remove the '-' if you want to see the dependency files generated.
-include $(patsubst %.c,build/%.d,$(SRC))

.PHONY: build all clean program size

# Rules for building the .eeprom rom images

#eeprom: ehex ebin esrec

#ehex:  $(PRG)_eeprom.hex
#ebin:  $(PRG)_eeprom.bin
#esrec: $(PRG)_eeprom.srec

#%_eeprom.hex: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
#	|| { echo empty $@ not generated; exit 0; }

#%_eeprom.srec: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
#	|| { echo empty $@ not generated; exit 0; }

#%_eeprom.bin: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
#	|| { echo empty $@ not generated; exit 0; }

# Everything below here is used by avr-libc's build system and can be ignored
# by the casual user.

#dox: eps png pdf
#
#eps: $(PRG).eps
#png: $(PRG).png
#pdf: $(PRG).pdf
#
#%.eps: %.fig
#   $(FIG2DEV) -L eps $< $@
#
#%.pdf: %.fig
#   $(FIG2DEV) -L pdf $< $@
#
#%.png: %.fig
#   $(FIG2DEV) -L png $< $@
#


#.PHONY: count
#.PHONY: clean
