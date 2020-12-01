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
#include "defines.h"

uint32_t batt_v_filt = 0;

static uint16_t batt_adc_get_voltage(void) {
  uint32_t adc_val;
  adc_val = ADCL;
  adc_val |= (ADCH<<8)&0x0300;
  if (adc_val == 0) {
    return 0;
  }
  uint32_t vcc = 1126400/adc_val;
  if (vcc > 65535) {
    return 65535;
  } else {
    return vcc;
  }
}

void batt_adc_enable(void) {

  // Enable clock to ADC
  cbi(PRR, PRADC);
  
  // Start ADC
  ADCSRB = 0x00;
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADSC);
  ADMUX = 0b00100001; // Vcc as ref. Mux select 1.1v ref.

  batt_v_filt = 0;

}

void batt_adc_disable(void) {

  // Disable ADC
  ADCSRA = 0x00;  
  // Disable clock to ADC
  sbi(PRR, PRADC);
}

uint32_t batt_v(void) {

  if (batt_v_filt < (1000UL * 128UL)) {
    batt_v_filt = ((uint32_t)(batt_adc_get_voltage())) << 7;
  } else {
    batt_v_filt = ((batt_v_filt * 127)>>7) + ((uint32_t)(batt_adc_get_voltage()));
  }
  return batt_v_filt>>7;

}