#ifndef __RTC_H__
#define __RTC_H__

#define RTC_MS_TO_32HZ_TICK(m) (m>>5)
#define RTC_S_TO_32HZ_TICK(s) (s<<5)

extern volatile uint16_t rtc_hour;
extern volatile uint8_t  rtc_min;
extern volatile uint8_t  rtc_sec;


void rtc_start(void);
void rtc_pps_set(uint8_t pps);
uint16_t rtc_get_subtick(void);
int16_t rtc_get_caib(void);
void rtc_set_calib(int16_t cal);
void delay_ms(uint16_t d);
void timeout_start(uint16_t * t);
uint8_t timeout_check_s(uint16_t * t, uint8_t s);
uint16_t rtc_toggle(uint16_t b);

void rtc_tick_test(void);



#endif