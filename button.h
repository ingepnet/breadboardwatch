#ifndef __BUTTON_H__
#define __BUTTON_H__

#define BTN_PRESSED      0x01
#define BTN_DOWN         0x02
#define BTN_UP           0x04
#define BTN_SHORT_CLICK  0x08
#define BTN_LONG_CLICK   0x10
#define BTN_LONG_PRESS   0x20

#define SHORT_CLICK_MIN_TIME RTC_MS_TO_32HZ_TICK(58)
#define SHORT_CLICK_MAX_TIME RTC_S_TO_32HZ_TICK(1)
#define LONG_PRESS_TIME      RTC_S_TO_32HZ_TICK(2)

extern volatile uint16_t btn_pcint;

uint8_t btn_status(void);
uint8_t btn_timeout(uint8_t * ks, uint16_t * t, uint8_t s);

#endif