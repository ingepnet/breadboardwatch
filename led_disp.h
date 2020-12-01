#ifndef __LED_DISP_H__
#define __LED_DISP_H__

#define segA _BV(2)
#define segB _BV(0)
#define segC _BV(6)|_BV(10)
#define segD _BV(3)
#define segE _BV(5)|_BV(7)
#define segF _BV(1)
#define segG _BV(4)

#define cat1 6
#define cat2 5
#define cat3 7
#define cat4 10
#define cat_mask (_BV(cat1)|_BV(cat2)|_BV(cat3)|_BV(cat4))

#if 1

extern volatile uint8_t port_in;

void led_drive_enable(void);
void led_drive_disable(void);
void led_set_digit(uint8_t digit, uint16_t segment);
void led_clear(void);
void led_chr(uint8_t digit, const char c);
void led_str(const char * str);
void led_uint_lead(uint16_t n);
void led_uint(uint16_t n);
void led_int(int16_t i);
void led_uint_l(uint16_t n);
void led_uint_h(uint16_t n);

#endif

#endif