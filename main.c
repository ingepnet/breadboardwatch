/*

Bread board watch - http://ingep.net/breadboardwatch/
--------------------------------------------------------------------------------

2020.11.29 - v1.0
- New menu functions:
  - Setting of RTC calibration, and storing to EEPROM
  - Readout of VCC
  - Reset of seconds to nearest minute


2017.10.14 - v0.1
- First revision vit RTC calibration hard coded.

--------------------------------------------------------------------------------
MIT License

Copyright (c) 2020 ingep.net

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------
 
*/

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "defines.h"
#include "rtc.h"
#include "led_disp.h"
#include "button.h"
#include "animation.h"
#include "menu.h"
#include "batt.h"


static void init_mcu(void) {

  // Power reduction register. Shut down USI and ADC
  PRR = 0b0011;
  // Disable analog comparator
  sbi(ACSR, ACD);

  // Start RTC
  rtc_start();

  led_str("v1.0");

  // Enaqble LED driver
  led_drive_enable();

  // Enable interrupts
  sei();


}

static void sleep_mcu(void) {

    cli();   
    led_drive_disable();
    DDRA = 0;
    DDRB = 0;
    PORTA = 0;
    
    #if 1
    PCMSK0 |= _BV(PCINT0);
    GIMSK  |= _BV(PCIE0);
    
    GIFR   |= _BV(PCIF0);
    GIFR   |= _BV(PCIF0);
    GIFR   |= _BV(PCIF0);
    GIFR   |= _BV(PCIF0);
    GIFR   |= _BV(PCIF0);
    btn_pcint = 0;
    #endif
    sei();
    
    while (btn_pcint == 0) {
    
      set_sleep_mode(SLEEP_MODE_IDLE);  
      sleep_enable();
      sleep_cpu();
      
    }
    
    #if 1
    GIMSK  &= ~(_BV(PCIE0));
    #endif      
    
    led_drive_enable(); 

}

int main(void) {

  init_mcu();
  batt_adc_enable();
  
  uint16_t tout, tout_anim;
  uint8_t bs;

  timeout_start(&tout);
  uint8_t state = 0;

  while(1) {
    
    if (state == 0) {

      // hours and minutes
      led_uint_h(rtc_hour);
      led_uint_l(rtc_min);
      
      // Show animation
      if (timeout_check_s(&tout_anim, 2)) {
        animation2(100);
        timeout_start(&tout_anim);
      }

    }

    // minutes and seconds
    if (state == 1) {      
      led_uint_l(rtc_sec);
      led_uint_h(rtc_min);      
    }

    // voltage
    if (state == 2) {
      led_uint(batt_v());
    }

    // Get button status
    if (btn_timeout(&bs, &tout, 120)) {      
      batt_adc_disable();
      
      animation1(100);
         
      sleep_mcu();
           
      animation1(30);

      batt_adc_enable();

      state = 0;

      timeout_start(&tout);
    } 
    
    // Enter menu on long press
    if (bs & BTN_LONG_PRESS) {
      menu();
      state = 0;
    }

    // Goto state 0 if no key pressed
    if (timeout_check_s(&tout, 30)) {
        state = 0;
    }
    
    if (bs & BTN_SHORT_CLICK) {
      state = (state+1)%3;
      
      switch (state) {
        case 0:
          animation2(50);
          timeout_start(&tout_anim);
          break;
        case 1:
          led_str("SEC ");
          break;
        case 2:
          led_str("BAt ");
          break;
      }
      delay_ms(200);
      
    }
    
  }; 
  
  
}
