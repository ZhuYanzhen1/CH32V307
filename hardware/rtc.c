//
// Created by LaoZhu on 2022/8/31.
//

#include "rtc.h"
#include "system.h"

static const unsigned char table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static unsigned char mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static unsigned char rtc_is_leap_year(u16 year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0)
                return 1;
            else
                return 0;
        } else
            return 1;
    } else
        return 0;
}

void rtc_set_time(unsigned short syear,
                  unsigned char smon,
                  unsigned char sday,
                  unsigned char hour,
                  unsigned char min,
                  unsigned char sec) {
    unsigned int t, seccount = 0;
    if (syear < 1970 || syear > 2099) {
        PRINTF_LOGE("RTC year must set between 1970~2099\r\n")
        return;
    }
    for (t = 1970; t < syear; t++) {
        if (rtc_is_leap_year(t))
            seccount += 31622400;
        else
            seccount += 31536000;
    }
    smon -= 1;
    for (t = 0; t < smon; t++) {
        seccount += (unsigned int) mon_table[t] * 86400;
        if (rtc_is_leap_year(syear) && t == 1)
            seccount += 86400;
    }
    seccount += (unsigned int) (sday - 1) * 86400;
    seccount += (unsigned int) hour * 3600;
    seccount += (unsigned int) min * 60;
    seccount += sec;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();
    PRINTF_LOGI("RTC time set to %d success\r\n", seccount)
}

void rtc_set_alarm(unsigned short syear,
                   unsigned char smon,
                   unsigned char sday,
                   unsigned char hour,
                   unsigned char min,
                   unsigned char sec) {
    unsigned short t;
    unsigned int seccount = 0;
    if (syear < 1970 || syear > 2099) {
        PRINTF_LOGE("Alarm year must set between 1970~2099\r\n")
        return;
    }
    for (t = 1970; t < syear; t++) {
        if (rtc_is_leap_year(t))
            seccount += 31622400;
        else
            seccount += 31536000;
    }
    smon -= 1;
    for (t = 0; t < smon; t++) {
        seccount += (unsigned int) mon_table[t] * 86400;
        if (rtc_is_leap_year(syear) && t == 1)
            seccount += 86400;
    }
    seccount += (unsigned int) (sday - 1) * 86400;
    seccount += (unsigned int) hour * 3600;
    seccount += (unsigned int) min * 60;
    seccount += sec;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetAlarm(seccount);
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    PRINTF_LOGI("Alarm set to %d success\r\n", seccount)
}

static unsigned char rtc_get_week(u16 year, unsigned char month, unsigned char day) {
    unsigned short temp2;
    unsigned char yearH, yearL;
    yearH = year / 100;
    yearL = year % 100;
    if (yearH > 19)
        yearL += 100;
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)
        temp2--;
    return (temp2 % 7);
}

void rtc_get_time(calendar_t *calendar) {
    static unsigned short daycnt = 0;
    unsigned int timecount, temp, temp1;
    timecount = RTC_GetCounter();
    temp = timecount / 86400;
    if (daycnt != temp) {
        daycnt = temp;
        temp1 = 1970;
        while (temp >= 365) {
            if (rtc_is_leap_year(temp1)) {
                if (temp >= 366)
                    temp -= 366;
                else {
                    temp1++;
                    break;
                }
            } else
                temp -= 365;
            temp1++;
        }
        calendar->year = temp1;
        temp1 = 0;
        while (temp >= 28) {
            if (rtc_is_leap_year(calendar->year) && temp1 == 1) {
                if (temp >= 29)
                    temp -= 29;
                else
                    break;
            } else {
                if (temp >= mon_table[temp1])
                    temp -= mon_table[temp1];
                else
                    break;
            }
            temp1++;
        }
        calendar->month = temp1 + 1;
        calendar->date = temp + 1;
    }
    temp = timecount % 86400;
    calendar->hour = temp / 3600;
    calendar->min = (temp % 3600) / 60;
    calendar->sec = (temp % 3600) % 60;
    calendar->week = rtc_get_week(calendar->year, calendar->month, calendar->date);
}

void utc_second_to_date_time(uint64_t utc_seconds, calendar_t *datetime) {
    uint64_t days_since_epoch, remainder;

    // Calculate days since epoch (1970-01-01)
    days_since_epoch = utc_seconds / 86400;
    remainder = utc_seconds % 86400;

    // Calculate time of day
    datetime->hour = remainder / 3600;
    remainder = remainder % 3600;
    datetime->min = remainder / 60;
    datetime->sec = remainder % 60;

    // Calculate year, month, and day
    datetime->year = 1970;
    while (days_since_epoch >= 365) {
        if ((datetime->year % 4 == 0 && datetime->year % 100 != 0) || datetime->year % 400 == 0) {
            if (days_since_epoch >= 366) {
                days_since_epoch -= 366;
                datetime->year++;
            } else
                break;
        } else {
            days_since_epoch -= 365;
            datetime->year++;
        }
    }

    if ((datetime->year % 4 == 0 && datetime->year % 100 != 0) || datetime->year % 400 == 0)
        mon_table[1] = 29;

    datetime->month = 1;
    while (days_since_epoch >= mon_table[datetime->month - 1]) {
        days_since_epoch -= mon_table[datetime->month - 1];
        datetime->month++;
    }

    datetime->date = days_since_epoch + 1; // Add 1 to get 1-based day of month
}

void rtc_config(void) {
    calendar_t build_date;
    unsigned char temp = 0;
    if ((BKP_ReadBackupRegister(BKP_DR1) & 0xFF00) != 0xA500) {
        PRINTF_LOGW("Backup region power down, reconfigure RCC\r\n")
        BKP_DeInit();
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 250) {
            temp++;
            delayms(20);
        }
        if (temp >= 250) {
            PRINTF_LOGE("LSE configure timeout\r\n")
            return;
        }
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        RTC_EnterConfigMode();
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();
        utc_second_to_date_time((uint64_t) CURRENT_TIME_UTC, &build_date);
        rtc_set_time(build_date.year, build_date.month, build_date.date,
                     build_date.hour, build_date.min, build_date.sec);
        RTC_ExitConfigMode();
        BKP_WriteBackupRegister(BKP_DR1, (0XA500 | (BKP_ReadBackupRegister(BKP_DR1) & 0x00FF)));
    } else {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        PWR_WakeUpPinCmd(DISABLE);
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
    }
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    PRINTF_LOGI("RTC configuration success, backup register1 is 0x%x\r\n", BKP_ReadBackupRegister(BKP_DR1))
}
