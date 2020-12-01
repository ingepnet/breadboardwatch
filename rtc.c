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
#include <avr/eeprom.h>

#include "rtc.h"
#include "led_disp.h"

// RTC clock counters
volatile uint16_t rtc_hour=0;
volatile uint8_t rtc_min=0;
volatile uint8_t rtc_sec=0;
volatile uint32_t rtc_tick=0;

// RTC calib
#define EEADDR_RTC_CALIB 0x000
#define RTC_CALIB_INTERVAL_MAX 65535
volatile static uint16_t calib_count=0;
volatile static int16_t rtc_calib_interval=RTC_CALIB_INTERVAL_MAX;
volatile static uint8_t rtc_calib_interval_neg=0;
volatile static int16_t rtc_calib = 0;
volatile static uint8_t rtc_pps = 0;

static void rtc_update_calib_interval(void) {

  uint16_t rc;

  if (rtc_calib < 0) {
    rtc_calib_interval_neg=1;
    rc = -rtc_calib;
  } else {
    rtc_calib_interval_neg=0;
    rc = rtc_calib;
  }

  if (rc == 0) {
    rtc_calib_interval = RTC_CALIB_INTERVAL_MAX;
    return;
  }

  rtc_calib_interval = 27000 / rc;

}

int16_t rtc_get_caib(void) {
  return rtc_calib;
}

void rtc_set_calib(int16_t cal) {
  rtc_calib = cal;
  rtc_update_calib_interval();
  eeprom_update_word((uint16_t *)EEADDR_RTC_CALIB, rtc_calib);
}

void rtc_pps_set(uint8_t pps) {
  rtc_pps = pps;
}



// c: calib interval
// h: tenth of a seconds in a day
// h = (((c + 1/32) / c) - 1) * 60 * 60 * 24 * 10
// c = (60 * 60 * 24 * 10) / (32 * h)
// c = 27000 / h


// RTC clock overflow interrupt routine @ 1Hz
ISR(TIM0_OVF_vect) {

  if (rtc_pps) {
    PORTA = 0;
    DDRA = _BV(3);
    PORTA = _BV(3);
    PORTA = 0;
  }

  calib_count++;  

  if (calib_count == 1) {
    // Normal speed
    OCR0A  = 31;  
  }

  if (calib_count >= rtc_calib_interval) {      
    if (rtc_calib_interval_neg) {
      // -31.25 ms
      OCR0A  = 30;
    } else {
      // +31.25 ms
      OCR0A  = 32;
    }      
    calib_count = 0;
  }

  // Increment RTC counters 
  rtc_sec++;
  if (rtc_sec >= 60) {
    rtc_min++;
    rtc_sec = 0;
    if (rtc_min >= 60) {
      rtc_hour++;
      rtc_min = 0;
      if (rtc_hour >= 24) {
        rtc_hour = 0;
      }
    }
  } 
  rtc_tick++;
    
}

void rtc_start(void) {

  // Load calib from EEPROM

  rtc_calib = eeprom_read_word((uint16_t *)EEADDR_RTC_CALIB);
  if (rtc_calib > 999) {
    rtc_calib = 0;
  }
  if (rtc_calib < -999) {
    rtc_calib = 0;
  }
  rtc_update_calib_interval();
  
  
  
  // Timer0 runs at 32Hz
  
  // Disable interrupt
  TIMSK0 = 0;
  
  // Setup counter
  OCR0A = 31;
  OCR0B = 0;
  
  // Select clock source
  TCCR0A = _BV(WGM01)|_BV(WGM00);
  TCCR0B = _BV(CS02)|_BV(CS00)|_BV(WGM02);
  //
  // Clear counter
  TCNT0 = 0;
  
  //Clear interrupt flags
  TIFR0 = _BV(OCF0B)|_BV(OCF0A)|_BV(TOV0);
  
  //Enable interrupt
  TIMSK0 = _BV(TOIE0); 
  
}

// LSB is 32Hz
uint8_t rtc_of = 0;
uint16_t rtc_get_subtick(void) {
  
  uint8_t tick_32, tick_32_;
  uint8_t tick_s;
  uint8_t tov0, tov0_;
  uint8_t retry = 5;
  
  cli(); 
  tick_s = rtc_tick;
  
  while (retry--) {
    tick_32 = TCNT0;  
    tov0 = TIFR0 & _BV(TOV0);
    tick_32_ = TCNT0;
    tov0_ = TIFR0 & _BV(TOV0);
    
    if ((tick_32 == tick_32_) && (tov0 == tov0_)) {  
      break;   
    }     
  }
    
  sei();
  
  if (retry == 255) {
    return 0;
  }
  
  if (tov0) {
    rtc_of = 1;
    tick_s++;
  } else {
    rtc_of = 0;
  }

  // tick_32 could be 32 when calibrating
  if (tick_32 > 31) {
    tick_32 = 31;
  }
  
  return (tick_s << 5) | (tick_32 & 0x1f);
    
}

void rtc_tick_test(void) {

  while(1) {

    uint16_t tick1 = rtc_get_subtick();
    uint8_t of1 = rtc_of;
    uint16_t tick2 = rtc_get_subtick();
    uint8_t of2 = rtc_of;
    uint16_t tick3 = rtc_get_subtick();
    uint8_t of3 = rtc_of;
    uint16_t tick4 = rtc_get_subtick();
    uint8_t of4 = rtc_of;

    if ((tick3 - tick2) > 64) {
        while (1) {
          led_str("t1  ");
          if (of1) led_chr(3,'o');
          delay_ms(500); 
          led_uint(tick1/10000);
          delay_ms(500); 
          led_uint_lead(tick1);
          delay_ms(500); 

          led_str("t2  ");
          if (of2) led_chr(3,'o');
          delay_ms(500); 
          led_uint(tick2/10000);
          delay_ms(500); 
          led_uint_lead(tick2);
          delay_ms(500);

          led_str("t3  ");
          if (of3) led_chr(3,'o');
          delay_ms(500); 
          led_uint(tick3/10000);
          delay_ms(500); 
          led_uint_lead(tick3);
          delay_ms(500); 

          led_str("t4  ");
          if (of4) led_chr(3,'o');
          delay_ms(500); 
          led_uint(tick4/10000);
          delay_ms(500); 
          led_uint_lead(tick4);
          delay_ms(500);
        }
    }  




  }



}

uint16_t rtc_toggle(uint16_t b) {
  return ((rtc_tick << 5) | (TCNT0 & 0x1f)) & b;  
}

void delay_ms(uint16_t d) {
  uint16_t t =  rtc_get_subtick();
  while (( rtc_get_subtick() - t) < RTC_MS_TO_32HZ_TICK(d));
}

void timeout_start(uint16_t * t) {
  *t =  rtc_get_subtick();
}

uint8_t timeout_check_s(uint16_t * t, uint8_t s) {
  uint16_t rt = rtc_get_subtick();
  if (( rt - *t) > (RTC_S_TO_32HZ_TICK((uint16_t)(s)))) {
      return 1;
  }
  return 0;  
}