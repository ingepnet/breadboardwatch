# Breadboard watch

http://ingep.net/breadboardwatch/

## Building and flashing

To build and flash the code using linux, the following packages are needed:
```
sudo apt install gcc-avr avr-libc avrdude
```

The fuses in the ATtiny84 needs to be changed to support a 32kHz crystal, this only needs to be done once:
```
make wfuse
```

Build the code:
```
make clean all
```

Write to flash:
```
make write
```

If programming is done in-circuit. Disconnecting the LED-display from PA6 (MOSI) may be necessary.


# FW description

Below is a description of the code running on the breadboard watch.

## Main - main.c

During normal operation the watch shows hour and minutes, with an animation every two seconds. If the button isn't pressed the watch will go into sleep mode after two minutes. Pressing the button in sleep mode activates the watch again.

A short click on the button activates the minute and second display. Another click displays the battery (VCC) voltage in mV. The display returns to hour and minute display in 20 seconds. 

## Menu - menu.c

The user interface of the watch is operated by a single button. By using the menu system it is possible to set the time and calibration. When in time display mode hold the button for two seconds to enter the menu. If no button is pressed, the menu will be exited in 20 seconds.

Five options are available in the main menu. Use a short click to cycle through, and a long click (more then two seconds) to enter the submenu.

1. "SEt " - Set time. The display toggles between option name "SEt" and the current time.
2. "SEC " - Show menu option reset seconds. The display toggles between option name "SEC" and the current minutes and seconds.
3. "CALI" - Show menu option RTC calibration.
4. "P   " - PPS puls on PA3 when this option is active, timout disabled on this menu entry.
5. "End " - Exit the menu.

### Option: "SEt " - Set time

This option is for setting the hour and minutes. First adust the minutes by using short click on the button. Then a long click to select the hour adjustment. Adjust the hours by short clicks also. A long click stores the changes. The seconds are set to zero during this setting.

### Option: "SEC " - Reset seconds

This option is for resetting the seconds. This is useful for syncing the watch to an accurate time source. One short click on the button resets the seconds to zero. If the seconds are past 30 when the button is pressed, the minutes are also incremented. This effectively rounds the time to the nearest minute. Exit this setting by a long press on the button.

### Option: "CALI" - RTC calibration

It is possible to set the RTC calibration with a resolution of 0.1sec/day. First select the sign of the calibration by short clicks on the button. Select a negative (-) sign if the watch is running to slow. Select a positive (blank) sign if the watch is running to fast. Use long clicks to select the next three digits of the calibration and short clicks to change them. A long click on the last digit saves the calibration value to EEPROM.

Examples:
  * If the RTC is running to 7.2 seconds to fast in a day, set the calibration value to " 072". 
  * If the RTC is running to 2.8 seconds to slow in a day, set the calibration value to "-028". 

### Option: "P   " - PPS pulse

When this main menu option is active/showing, a PPS pulse is available on the PA3 port. Timeout is disabled on this menu entry.

### Option: "End " - Exit

A long press on this main menu option exits the menu.


## Real Time Clock - rtc.c

The 8-bit Timer/Counter0 is used as a RTC. The counter is running at 32Hz by using a divide by 1024 pre-scaler.
When the counter reaches 31 the counter is reset back to zero and an interrupt is generated. This gives an interrupt rate of 1Hz for the RTC IRQ. The hour, minute and second counters are incremented in this interrupt routine (TIM0_OVF_vect).

The RTC also has a calibration function. This can compensate for the 32kHz crystal running a bit to fast or slow. Since the RTC counter is running at 32Hz, the smallest possible adjustment unit is 1/32Hz = 31.25ms. Depending on a to fast or to slow crystal, the top value of the counter is set to 30 or 32, to make the RTC loose or gain 31.25ms respectively. By selecting the interval at which this 31.25ms adjustment is done, the amount of calibration is varied. For instance adding 31.25ms to a single interrupt period every 300 periode, makes the RTC run 9 seconds slower in a day.

It is possible to get the PPS puls from the watch on PA3, by selecting the PPS mode in the menu. The PPS puls will be available as long as this menu option is selected. This can be used check the drifting of the 32kHz crystal compared to a GPS PPS.

## LED display driver - led_disp.c

The LED display is updated in a 256Hz interrupt routine. With four multiplexed digits this gives a refresh rate of 64Hz. To make the interrupt routing as fast as possible, the port values are pre-calculated. The routine only has to copy these values in to the port registers. The state of the button is also sampled in the ISR, because the button shares a GPIO with the LED-display.





Copyright (c) 2020 ingep.net







