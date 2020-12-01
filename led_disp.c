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

#include "led_disp.h"

volatile uint8_t port_in=0;



#if 0
// Registers could be used to store global port data to save time in LED-display update interrupt routine.
// This makes interrupt routine use 23% of time instead of 30%.
// CFLAGS -ffixed-reg must be specified in makefile if this is used.
register uint8_t porta_d0 asm ("r2");
register uint8_t porta_d1 asm ("r3");
register uint8_t porta_d2 asm ("r4");
register uint8_t porta_d3 asm ("r5");
register uint8_t ddra_d0  asm ("r6");
register uint8_t ddra_d1  asm ("r7");
register uint8_t ddra_d2  asm ("r8");
register uint8_t ddra_d3  asm ("r9");
register uint8_t portb_d0 asm ("r10");
register uint8_t portb_d1 asm ("r11");
register uint8_t portb_d2 asm ("r12");
register uint8_t portb_d3 asm ("r13");
register uint8_t ddrb_d0  asm ("r14");
register uint8_t ddrb_d1  asm ("r15");
register uint8_t ddrb_d2  asm ("r16");
register uint8_t ddrb_d3  asm ("r17");
#else
// Variables to store port data to be loaded in display update interrupt routine.
uint8_t porta_d0, porta_d1, porta_d2, porta_d3;
uint8_t ddra_d0, ddra_d1, ddra_d2, ddra_d3;
uint8_t portb_d0, portb_d1, portb_d2, portb_d3;
uint8_t ddrb_d0, ddrb_d1, ddrb_d2, ddrb_d3;
#endif

volatile static uint8_t active_digit = 0;

// LED segment look-up-table
#define SEGMENT_LUT_SIZE 95
static uint16_t segment_lut[SEGMENT_LUT_SIZE] = { 
  0,                                  // 
  segB|segD,                          //!
  segB|segF,                          //"
  segA|segB|segF|segG,                //#
  segA|segC|segD|segF|segG,           //$
  segA|segC|segD|segF|segG,           //%
  segA|segC|segE|segF|segG,           //&
  segB,                               //'
  segB|segC|segG,                     //(
  segA|segB|segC|segD,                //)
  segA|segG,                          //*
  segE|segF|segG,                     //+
  segC|segD,                          //,
  segG,                               //-
  segD,                               //.
  segB|segE|segG,                     ///
  segA|segB|segC|segD|segE|segF,      //0
  segB|segC,                          //1
  segA|segB|segD|segE|segG,           //2
  segA|segB|segC|segD|segG,           //3
  segB|segC|segF|segG,                //4
  segA|segC|segD|segF|segG,           //5
  segA|segC|segD|segE|segF|segG,      //6
  segA|segB|segC,                     //7
  segA|segB|segC|segD|segE|segF|segG, //8
  segA|segB|segC|segD|segF|segG,      //9
  segC|segF,                          //:
  segC|segD|segF,                     //;
  segD|segE|segG,                     //<
  segD|segG,                          //=
  segC|segD|segG,                     //>
  segA|segB|segD,                     //?
  segA|segB|segC|segD|segE|segG,      //@
  segA|segB|segC|segE|segF|segG,      //A
  segC|segD|segE|segF|segG,           //B
  segA|segD|segE|segF,                //C
  segB|segC|segD|segE|segG,           //D
  segA|segD|segE|segF|segG,           //E
  segA|segE|segF|segG,                //F
  segA|segC|segD|segE|segF,           //G
  segB|segC|segE|segF|segG,           //H
  segE|segF,                          //I
  segB|segC|segD|segE,                //J
  segB|segC|segE|segF|segG,           //K
  segD|segE|segF,                     //L
  segA|segB|segC|segE|segF,           //M
  segA|segB|segC|segE|segF,           //N
  segA|segB|segC|segD|segE|segF,      //O
  segA|segB|segE|segF|segG,           //P
  segA|segB|segC|segF|segG,           //Q
  segE|segG,                          //R
  segA|segC|segD|segF|segG,           //S
  segA|segE|segF,                     //T
  segB|segC|segD|segE|segF,           //U
  segB|segC|segD|segE|segF,           //V
  segB|segC|segD|segE|segF|segG,      //W
  segB|segC|segE|segF|segG,           //X
  segB|segE|segF|segG,                //Y
  segA|segB|segD|segE|segG,           //Z
  segA|segD|segE|segF,                //[
  segC|segF|segG,                     //\ */
  segA|segB|segC|segD,                //]
  segA|segB|segF,                     //^
  segD,                               //_
  segF,                               //`
  segA|segB|segC|segE|segF|segG,      //a
  segC|segD|segE|segF|segG,           //b
  segD|segE|segG,                     //c
  segB|segC|segD|segE|segG,           //d
  segA|segB|segD|segE|segF|segG,      //e
  segA|segE|segF|segG,                //f
  segA|segB|segC|segD|segF|segG,      //g
  segC|segE|segF|segG,                //h
  segE,                               //i
  segA|segF|segG,                     //j
  segB|segC|segE|segF|segG,           //k
  segD|segE,                          //l
  segC|segE|segG,                     //m
  segC|segE|segG,                     //n
  segC|segD|segE|segG,                //o
  segA|segB||segE|segF|segG,          //p
  segA|segB|segC|segF|segG,           //q
  segE|segG,                          //r
  segA|segC|segD|segF|segG,           //s
  segD|segE|segF|segG,                //t
  segC|segD|segE,                     //u
  segC|segD|segE,                     //v
  segC|segD|segE,                     //w
  segB|segC|segE|segF|segG,           //x
  segB|segC|segD|segF|segG,           //y
  segD|segG,                          //z
  segB|segC|segG,                     //{
  segE|segF,                          //|
  segE|segF|segG,                     //}
  segB|segG                           //~
};

#define segment_num_lut(i) segment_lut['0'-' '+i]

static uint8_t digit_lut[4] = {
  cat1,
  cat2,
  cat3,
  cat4  
};

static uint8_t digit_pair[4] = {
  cat4,
  cat3,
  cat2,
  cat1  
};

static uint8_t digit_strange[4] = {
  cat2,
  cat1,
  cat4,
  cat3  
};

uint16_t segment_ascii_lut(unsigned char c) {
  if ((c-' ') > SEGMENT_LUT_SIZE) {
    return 0;
  }

  return segment_lut[c-' '];


}



// LED display update interrupt routine 
ISR(TIM1_OVF_vect) {
  

  // Set all ports to inputs and low
  DDRA = 0;
  DDRB = 0;
  PORTA = 0;
  
  // Read button port
  port_in = PINA; // Dummy
  port_in = PINA; // Dummy
  port_in = PINA;

  // Write active digit to ports
  uint8_t d = active_digit;
  
  if (d == 0) {  
    PORTA = porta_d0;
    PORTB = portb_d0;
    DDRA  = ddra_d0;
    DDRB  = ddrb_d0;
  }
  
  if (d == 1) {  
    PORTA = porta_d1;
    PORTB = portb_d1;
    DDRA  = ddra_d1;
    DDRB  = ddrb_d1;
  }
  
  if (d == 2) {  
    PORTA = porta_d2;
    PORTB = portb_d2;
    DDRA  = ddra_d2;
    DDRB  = ddrb_d2;
  }
  
  if (d == 3) {  
    PORTA = porta_d3;
    PORTB = portb_d3;
    DDRA  = ddra_d3;
    DDRB  = ddrb_d3;
  }
  
  // Select next digit
  active_digit = (active_digit+1)&0x3;
    
}

void led_drive_enable(void) {

  // 256Hz Timer1 overflow interrupt
  
  // Disable interrupt
  TIMSK1 = 0;
  
  // Setup counter
  OCR1AH = 0;
  OCR1AL = 1;
  OCR1BH = 0;
  OCR1BL = 0;
  
  // Select clock source
  TCCR1A = _BV(WGM11)|_BV(WGM10);
  TCCR1B = _BV(WGM13)|_BV(WGM12)|_BV(CS11)|_BV(CS10);

  // Clear counter
  TCNT1H = 0;
  TCNT1L = 0;
  
  //Clear interrupt flags
  TIFR1 = _BV(OCF1B)|_BV(OCF1A)|_BV(TOV1);
  
  //Enable interrupt
  TIMSK1 = _BV(TOIE1); 
  
  
}

void led_set_digit(uint8_t digit, uint16_t segment) {

  if (digit > 3) return;
  
  uint16_t port = segment & ~_BV(digit_lut[digit]);
  
  // All to output
  uint16_t ddr  = 0x4ff;
  
  // All cat to input
  ddr &= ~cat_mask;
  
  // Active cat output
  ddr |= _BV(digit_lut[digit]);
  
  // Enable output on digit pair if needed
  if (_BV(digit_lut[digit]) & segment) {
    ddr |= _BV(digit_pair[digit]);
  }
  
  // Enable output on digit other pair if needed
  if ((~ddr) & port) {
    ddr |= _BV(digit_strange[digit]);
  } 
  
  if (digit == 0) {
    cli();
    porta_d0 = port;
    portb_d0 = port>>8; 
    ddra_d0 = ddr;
    ddrb_d0 = ddr>>8; 
    sei(); 
  }
  
  if (digit == 1) {
    cli();
    porta_d1 = port;
    portb_d1 = port>>8; 
    ddra_d1 = ddr;
    ddrb_d1 = ddr>>8;
    sei();  
  }
  
  if (digit == 2) {
    cli();
    porta_d2 = port;
    portb_d2 = port>>8; 
    ddra_d2 = ddr;
    ddrb_d2 = ddr>>8;
    sei();  
  }
  
  if (digit == 3) {
    cli();
    porta_d3 = port;
    portb_d3 = port>>8; 
    ddra_d3 = ddr;
    ddrb_d3 = ddr>>8; 
    sei(); 
  } 
  
}

void led_clear(void) {
  led_set_digit(0, 0);
  led_set_digit(1, 0);
  led_set_digit(2, 0);
  led_set_digit(3, 0);
}

void led_drive_disable(void) {

  // Disable interrupt
  TIMSK1 = 0;
  
  // Select clock source
  TCCR1A = 0;
  TCCR1B = 0;

  // Clear counter
  TCNT1H = 0;
  TCNT1L = 0;
  
  //Clear interrupt flags
  TIFR1 = _BV(OCF1B)|_BV(OCF1A)|_BV(TOV1);
  
  led_clear();
  
}

void led_chr(uint8_t digit, const char c) {
  led_set_digit(digit, segment_ascii_lut(c));
}

void led_str(const char * str) {
  led_set_digit(0, segment_ascii_lut(str[0]));
  led_set_digit(1, segment_ascii_lut(str[1]));
  led_set_digit(2, segment_ascii_lut(str[2]));
  led_set_digit(3, segment_ascii_lut(str[3]));
}

void led_uint_lead(uint16_t n) {
  
  led_set_digit(3, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(2, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(1, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(0, segment_num_lut(n % 10)); 
}

void led_uint(uint16_t n) {
  
  led_set_digit(3, segment_num_lut(n % 10));
  n /= 10; 
  if (n) led_set_digit(2, segment_num_lut(n % 10)); else led_set_digit(2,0);
  n /= 10; 
  if (n) led_set_digit(1, segment_num_lut(n % 10)); else led_set_digit(1,0);
  n /= 10; 
  if (n) led_set_digit(0, segment_num_lut(n % 10)); else led_set_digit(0,0); 
}

void led_int(int16_t i) {

  if (i < -999) {
    led_str("-ERR");
    return;
  }

  uint16_t n;

  if (i < 0) {
    n = -i;
  } else {
    n = i;
  }
  
  led_set_digit(3, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(2, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(1, segment_num_lut(n % 10));

  if (i < 0) {
    led_set_digit(0, segment_ascii_lut('-'));
  } else {
    led_set_digit(0, 0);
  }
}

void led_uint_l(uint16_t n) { 
  led_set_digit(3, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(2, segment_num_lut(n % 10));
}

void led_uint_h(uint16_t n) {
  led_set_digit(1, segment_num_lut(n % 10));
  n /= 10; 
  led_set_digit(0, segment_num_lut(n % 10));
}
