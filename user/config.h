//
// Created by LaoZhu on 2022/8/20.
//

#ifndef CH32V307VC_USER_CONFIG_H_
#define CH32V307VC_USER_CONFIG_H_

#define FREERTOS_HEAP_SIZE          (16 * 1024)

/* 0: don't print any debug info
 * 1: print only error info
 * 2: print warning and error info
 * 3: print all debug info
 * */
#define PRINT_DEBUG_LEVEL           3

#define STARTUP_CLEAR_SCREEN        1

#define DEBUG_SERIAL_BAUDRATE       115200

#if DEBUG_SERIAL_BAUDRATE > 230400
#error "WCH-Link maximum baudrate is 230400, the baud rate cannot be set to a number greater than 230400"
#endif

#endif //CH32V307VC_USER_CONFIG_H_
