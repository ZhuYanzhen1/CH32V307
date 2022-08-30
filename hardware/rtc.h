//
// Created by LaoZhu on 2022/8/31.
//

#ifndef CH32V307VC_HARDWARE_RTC_H_
#define CH32V307VC_HARDWARE_RTC_H_

typedef struct {
    volatile unsigned char hour;
    volatile unsigned char min;
    volatile unsigned char sec;
    volatile unsigned short year;
    volatile unsigned char month;
    volatile unsigned char date;
    volatile unsigned char week;
} calendar_t;

void rtc_config(void);
void rtc_get_time(calendar_t *calendar);
void rtc_set_alarm(unsigned short syear,
                   unsigned char smon,
                   unsigned char sday,
                   unsigned char hour,
                   unsigned char min,
                   unsigned char sec);
void rtc_set_time(unsigned short syear,
                  unsigned char smon,
                  unsigned char sday,
                  unsigned char hour,
                  unsigned char min,
                  unsigned char sec);

#endif //CH32V307VC_HARDWARE_RTC_H_
