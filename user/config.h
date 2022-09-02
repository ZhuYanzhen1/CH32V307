//
// Created by LaoZhu on 2022/8/20.
//

#ifndef CH32V307VC_USER_CONFIG_H_
#define CH32V307VC_USER_CONFIG_H_

#define DEBUG_SERIAL_BAUDRATE       115200
#define FREERTOS_HEAP_SIZE          16 * 1024

/* 0: don't print any debug info
 * 1: print only error info
 * 2: print warning and error info
 * 3: print all debug info
 * */
#define PRINT_DEBUG_LEVEL           3

#define STARTUP_CLEAR_SCREEN        1

#endif //CH32V307VC_USER_CONFIG_H_
