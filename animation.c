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

#include <util/delay.h>
#include <avr/io.h>
#include "led_disp.h"

void animation1(uint16_t d) {  
  led_clear(); 
  led_set_digit(0, segA);_delay_loop_2(d);
  led_set_digit(1, segA);_delay_loop_2(d);
  led_set_digit(2, segA);_delay_loop_2(d);
  led_set_digit(3, segA);_delay_loop_2(d);
  led_set_digit(3, segA|segB);_delay_loop_2(d);
  led_set_digit(3, segA|segB|segC);_delay_loop_2(d);
  led_set_digit(3, segA|segB|segC|segD);_delay_loop_2(d);
  led_set_digit(2, segA|segD);_delay_loop_2(d);
  led_set_digit(1, segA|segD);_delay_loop_2(d);
  led_set_digit(0, segA|segD);_delay_loop_2(d);
  led_set_digit(0, segA|segE|segD);_delay_loop_2(d);
  led_set_digit(0, segA|segE|segF|segD);_delay_loop_2(d);
  led_set_digit(0, segE|segF|segD);_delay_loop_2(d);
  led_set_digit(1, segD);_delay_loop_2(d);
  led_set_digit(2, segD);_delay_loop_2(d);
  led_set_digit(3, segB|segC|segD);_delay_loop_2(d);
  led_set_digit(3, segC|segD);_delay_loop_2(d);
  led_set_digit(3, segD);_delay_loop_2(d);
  led_set_digit(3, 0);_delay_loop_2(d);
  led_set_digit(2, 0);_delay_loop_2(d);
  led_set_digit(1, 0);_delay_loop_2(d);
  led_set_digit(0, segE|segF);_delay_loop_2(d);
  led_set_digit(0, segF);_delay_loop_2(d);
  led_set_digit(0, 0);_delay_loop_2(d);  
}


void animation2(uint16_t d) {  
  led_clear(); 
  led_set_digit(0, segA);_delay_loop_2(d);
  led_set_digit(0, 0);led_set_digit(1, segA);_delay_loop_2(d);
  led_set_digit(1, 0);led_set_digit(2, segA);_delay_loop_2(d);
  led_set_digit(2, 0);led_set_digit(3, segA);_delay_loop_2(d);
  led_set_digit(3, segB);_delay_loop_2(d);
  led_set_digit(3, segC);_delay_loop_2(d);
  led_set_digit(3, segD);_delay_loop_2(d);
  led_set_digit(3, 0);led_set_digit(2, segD);_delay_loop_2(d);
  led_set_digit(2, 0);led_set_digit(1, segD);_delay_loop_2(d);
  led_set_digit(1, 0);led_set_digit(0, segD);_delay_loop_2(d);
  led_set_digit(0, segE);_delay_loop_2(d);
  led_set_digit(0, segF);_delay_loop_2(d);
  led_set_digit(0, 0);_delay_loop_2(d);  
}