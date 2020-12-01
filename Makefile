MCU = attiny84
F_CPU = 32768
TARGET = bbwatch
SRC = main.c rtc.c led_disp.c button.c animation.c menu.c batt.c
OBJ = $(SRC:.c=.o)

CFLAGS  = -DF_CPU=$(F_CPU)UL -g -Os -std=gnu99
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst) -mmcu=$(MCU) -I.
#CFLAGS += -ffixed-r2 -ffixed-r3 -ffixed-r4 -ffixed-r5 -ffixed-r6 -ffixed-r7 -ffixed-r8 -ffixed-r9 -ffixed-r10 -ffixed-r11 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-r16 -ffixed-r17 

AVRDUDE = avrdude -v -p attiny84 -c avrisp2 -P usb -B 200

%.o : %.c
	avr-gcc -c $(CFLAGS) $< -o $@ 

all: $(OBJ)
	avr-gcc $(CFLAGS) $(OBJ) --output $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex


rfuse:
	$(AVRDUDE) -U lfuse:r:-:h -U hfuse:r:-:h
	
wfuse:
	$(AVRDUDE) -U lfuse:w:0xe6:m -U hfuse:w:0xd7:m -U efuse:w:0xff:m

write:
	$(AVRDUDE) -U flash:w:$(TARGET).hex

read:
	$(AVRDUDE) -F -U flash:r:-:h


clean: 
	rm -f $(TARGET).hex $(TARGET).elf $(SRC:.c=.lst) $(SRC:.c=.o)

.PHONY : all clean


