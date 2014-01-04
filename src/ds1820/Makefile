# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# Makefile for the avr-gcc Toolchain
#
# by Steffen Vogel <post@steffenvogel.de>
#
#----------------------------------------------------------------------------


# Target Options
TARGET = main

OBJS = $(TARGET).o ds1820.o lcd.o

# Target
MCU = atmega16
F_CPU = 14745000

# Programming
AVRDUDE_FLAGS = -p m16 -c usbasp -P usb

HFUSE=
LFUSE=

DEFINES = -DF_CPU=$(F_CPU) -DBAUD=115200
WARNINGS = -Wall -Wextra

CPPFLAGS += -MD -MP
CXXFLAGS +=
CFLAGS += -g -O2 -mmcu=$(MCU) $(DEFINES) $(WARNINGS)
LDFLAGS += -Wl,-Map,$(TARGET).map -Wl,-u,vfprintf -lprintf_flt -lm

# Tools
AR = avr-ar
AS = avr-as
CC = avr-gcc
CXX = avr-g++
NM = avr-nm
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AVRDUDE = avrdude

# Dependencies
-include $(OBJS:.o=.d)

# Default Target
all: $(TARGET).hex $(TARGET).eep $(TARGET).lst

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eep: %.elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@

flash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex

eeprom: $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:$(TARGET).eep

fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

size: $(TARGET).elf
	$(SIZE) -A $(TARGET).elf --format=avr --mcu $(MCU) | tail -n +5
	$(NM) --print-size --size-sort --reverse-sort --radix=d main.elf --format=s | tail -n +5 | head -n 12

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(TARGET).elf
	rm -f $(TARGET).hex
	rm -f $(TARGET).eep
	rm -f $(TARGET).lst
	rm -f $(TARGET).map

.PHONY: all size clean flash eeprom fuses
