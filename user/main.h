//
// Created by LaoZhu on 2022/8/20.
//

#ifndef CH32V307VC_USER_MAIN_H_
#define CH32V307VC_USER_MAIN_H_

////////////////////////// Operating System //////////////////////////
#include "system.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

////////////////////////// File System //////////////////////////
#include "ff.h"
#include "fswrap.h"

////////////////////////// Hardware //////////////////////////
#include "adc.h"
#include "usart.h"
#include "rtc.h"
#include "gpio.h"
#include "iic.h"
#include "spi.h"

////////////////////////// Device //////////////////////////
#include "at24c02.h"
#include "w25qxx.h"

#endif //CH32V307VC_USER_MAIN_H_
