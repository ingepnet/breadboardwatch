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

#include "rtc.h"
#include "led_disp.h"
#include "button.h"
#include "defines.h"
#include "menu.h"

uint32_t prev_rtc_tick=0;
uint32_t prev_sec_since_midnight=0;

int16_t roll_digit(int16_t num, uint8_t digit, int8_t roll) {

  if (digit > 3) {
    return num;
  }

  int16_t sign=1;

  if (num < 0) {
    sign = -1;
    num = -num;
  }

  int16_t digits[4] = {
    (num/1   ) % 10,
    (num/10  ) % 10,
    (num/100 ) % 10,
    (num/1000) % 10
  };

  digits[digit] = (digits[digit] + roll)%10;

  return sign*(digits[0] + digits[1] * 10 + digits[2] * 100 + digits[3] * 1000);

}

/*
 * 
 *  - Menu
 *    - Set time
 *      - Set min
 *      - Set hour
 *          
 *       
 * 
 * 
 * 
 */


void menu_set_time(void) {
  
  uint16_t tout;
  uint8_t ks;

  timeout_start(&tout);
  
  uint8_t set_index = 0;
  
  cli();
  uint8_t set_hour = rtc_hour;
  uint8_t set_min  = rtc_min; 
  sei();
  
  while (1) {

    if (btn_timeout(&ks, &tout, 120)) {
      break;
    } 
  
    // Set min
    if (set_index == 0) {
      
      if (ks & BTN_SHORT_CLICK) {
        set_min = (set_min+1)%60;   
      }     
      
      led_uint_h(set_hour);
      
      if (rtc_toggle(0x18)) {
        led_uint_l(set_min);
      } else {
        led_set_digit(2,0);
        led_set_digit(3,0);
      }   
    }
    
    // Set hour
    if (set_index == 1) {
      
      if (ks & BTN_SHORT_CLICK) {
        set_hour = (set_hour+1)%24;   
      }
      
      led_uint_l(set_min);
      
      if (rtc_toggle(0x18)) {
        led_uint_h(set_hour);
      } else {
        led_set_digit(0,0);
        led_set_digit(1,0);
      }   
    }
    
    if (ks & BTN_LONG_PRESS) {
      set_index++;
    }
    
    if (set_index >= 2) {
      cli();
      rtc_hour = set_hour;
      rtc_min = set_min;
      rtc_sec = 0;
      sei();
      break;
    }  
    
  }

  

}

void menu_set_calib(void) {
  
  uint16_t tout;
  uint8_t ks;

  timeout_start(&tout);

  int16_t set_rtc_calib;

  int16_t calib = rtc_get_caib();
  
  if (calib < -999) {
    set_rtc_calib = -999;
  } else if (calib > 999) {
    set_rtc_calib = 999;
  } else {
    set_rtc_calib = calib;
  }
  uint8_t set_index = 0;

  while (1) {

    if (btn_timeout(&ks, &tout, 120)) {
      return;
    } 
  
    if (ks & BTN_SHORT_CLICK) {
      if ((set_index > 0) && (set_index <= 3)) set_rtc_calib = roll_digit(set_rtc_calib, 3-set_index, 1);
      if (set_index == 0) set_rtc_calib = -set_rtc_calib;
    }

    if (rtc_toggle(0x01c)) {
      led_int(set_rtc_calib);      
    } else {      
      led_chr(set_index,'_');
    }

    if (ks & BTN_LONG_PRESS) {
      set_index++;
    }
    
    if (set_index >= 4) {
      rtc_set_calib(set_rtc_calib);
      led_str("SAVE");
      delay_ms(2000);
      break;
    }  
    
  }

  

}

void menu_reset_sec(void) {
  
  uint16_t tout;
  uint8_t ks;

  timeout_start(&tout);

  //uint32_t rtc_tick_now=0;
  //uint32_t sec_since_midnight=0;

  while (1) {

    if (btn_timeout(&ks, &tout, 120)) {
      return;
    } 
  
    if (ks & BTN_SHORT_CLICK) {
      //rtc_tick_now = rtc_get_subtick();
      cli();
      if (rtc_sec > 30) {
        rtc_min++;        
        if (rtc_min >= 60) {
          rtc_hour++;
          rtc_min = 0;
          if (rtc_hour >= 24) {
            rtc_hour = 0;
          }
        }
      }
      rtc_sec = 0;
      //sec_since_midnight = ((rtc_hour * 60)+rtc_min)*60;
      sei();
    }

    if (rtc_toggle(0x01c)) {
      led_uint_h(rtc_min);
      led_uint_l(rtc_sec);
    } else {      
      led_set_digit(2,0);
      led_set_digit(3,0);
      led_uint_h(rtc_min);
    }

    if (ks & BTN_LONG_PRESS) {
      led_str("done");
      delay_ms(2000);
      break;
    } 
    
  }

  #if 0
  // Attemt at calculating RTC calibration based on reset of seconds
  // A "day" counter needs to be added in the RTC.

  if (rtc_tick_now) {
    if (prev_rtc_tick) {
      uint32_t days = (rtc_tick_now - prev_rtc_tick) / 2764800; // 24*60*60*32
      uint32_t sec_since_prev_reset_midnight = sec_since_midnight + (days*86400);
      int32_t diff_tick = (rtc_tick_now - prev_rtc_tick)-(sec_since_prev_reset_midnight-prev_sec_since_midnight)*32;

      if (diff_tick > 1382400) { // 12*60*60*32
        sec_since_prev_reset_midnight += 86400;
        diff_tick = (rtc_tick_now - prev_rtc_tick)-(sec_since_prev_reset_midnight-prev_sec_since_midnight)*32;
      } else if (diff_tick < -1382400) {
        sec_since_prev_reset_midnight -= 86400;
        diff_tick = (rtc_tick_now - prev_rtc_tick)-(sec_since_prev_reset_midnight-prev_sec_since_midnight)*32;
      }

      int32_t diff_sec = diff_tick / 32;

      if ((diff_sec < 999) && (diff_sec > -999)) {
        led_int(diff_sec);
      } else {
        led_str("OF  ");
      }

      delay_ms(5000);


    }
    prev_rtc_tick = rtc_tick_now;
    prev_sec_since_midnight = sec_since_midnight;
  }
  #endif

  

}



void menu(void) {

  uint16_t tout;
  uint8_t ks;
  
  led_str("CONF");

  delay_ms(2000);  

  timeout_start(&tout);

  uint8_t conf_index = 0;

      
  while (1) {

    if (btn_timeout(&ks, &tout, 20)) {
      break;
    } 
  
    if (conf_index == 0) {

      if (rtc_toggle(1<<4)) {
        led_str("SEt ");
      } else {
        led_uint_lead((rtc_hour*100) + rtc_min);
      }
      
      // Set time 
      if (ks & BTN_LONG_PRESS) {   
        menu_set_time();
        timeout_start(&tout);
        break;
      }

    }

    if (conf_index == 1) {
      if (rtc_toggle(1<<4)) {
        led_str("SEC ");
      } else {
        led_uint_h(rtc_min);
        led_uint_l(rtc_sec);
      }
      if (ks & BTN_LONG_PRESS) {   
        menu_reset_sec();
        timeout_start(&tout);
      }

    }

    if (conf_index == 2) {
      if (rtc_toggle(1<<4)) {
        led_str("CALI");
      } else {
        led_int(rtc_get_caib());
      }
      if (ks & BTN_LONG_PRESS) {   
        menu_set_calib();
        timeout_start(&tout);
      }

    }

    if (conf_index == 3) {
      rtc_pps_set(1);
      timeout_start(&tout);
      if (rtc_toggle(1<<4)) {
        led_str("P   ");
      } else {
        led_str("    ");
      }
    } else {
      rtc_pps_set(0);
    }

    if (conf_index == 4) {
      if (rtc_toggle(1<<4)) {
        led_str("End ");
      } else {
        led_str("End_");
      }
      if (ks & BTN_LONG_PRESS) {   
        break;
      }

    }

    if (ks & BTN_SHORT_CLICK) {
      conf_index = (conf_index+1)%5;
    } 
    
    
  
  }

  rtc_pps_set(0);
  
  
  
}