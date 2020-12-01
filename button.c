/*

Bread board watch - http://ingep.net/breadboardwatch/
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
#include <avr/interrupt.h>
#include "button.h"
#include "led_disp.h"
#include "rtc.h"

// Pin change interrupt
volatile uint16_t btn_pcint = 0;
ISR(PCINT0_vect) {  
  btn_pcint = 1;
}

/*
 * 
 * Pushbutton functions 
 * 
 */


// Get pushbutton state
uint8_t static inline btn_down(void) {
  if (port_in & 0x01) {
    return 1;
  } else {
    return 0;
  }
}



uint16_t btn_down_time = 0;
uint8_t  last_btn_stat;
uint8_t  btn_stat_;
uint8_t  long_press_detect = 0;

uint8_t btn_status(void) {
  
  uint8_t btn_flags = 0;
  
  uint8_t btn_stat = btn_down();
  
  btn_stat_ = btn_stat;
  
  uint16_t tick_32 = rtc_get_subtick();
  
  // Key down
  if ((last_btn_stat == 0) && btn_stat) {
    btn_down_time = tick_32;
    long_press_detect = 0;
    btn_flags |= BTN_DOWN;
  }
  
  // Key pressed
  if (btn_stat) {
    btn_flags |= BTN_PRESSED;
    
    // Long press detect
    if (long_press_detect == 0) {
      if ((tick_32 - btn_down_time) > LONG_PRESS_TIME) {
        btn_flags |= BTN_LONG_PRESS;
        long_press_detect = 1;  
      }  
    } 
    
  }

  
  // Key up
  if (last_btn_stat  && (btn_stat == 0)) {
    btn_flags |= BTN_UP;
    
    // Long click
    if (long_press_detect) {
      btn_flags |= BTN_LONG_CLICK;
    } else {
    
      // Short click;
      if (((tick_32 - btn_down_time) > SHORT_CLICK_MIN_TIME) &&
        ((tick_32 - btn_down_time) < SHORT_CLICK_MAX_TIME)
      ) {
        btn_flags |= BTN_SHORT_CLICK;
      }
      
    }
    
  } 
  
  last_btn_stat = btn_stat; 
  
  return btn_flags;
  
}

uint8_t btn_timeout(uint8_t * ks, uint16_t * t, uint8_t s) {
    
    *ks = btn_status();     
    // Reset timeout
    if (*ks & BTN_PRESSED) {
      timeout_start(t);
    }    
    return timeout_check_s(t, s);
}