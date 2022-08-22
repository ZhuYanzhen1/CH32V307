#ifndef __DEBUG_H
#define __DEBUG_H

#include "printf.h"
#include "ch32v30x.h"
#include "config.h"

#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)

#if PRINT_DEBUG_LEVEL > 0
#if PRINT_DEBUG_LEVEL >= 3
#define PRINTF_LOGI     printf("I(%d): ", global_system_time_stamp);printf
#else
#define PRINTF_LOGI     (void)
#endif
#if PRINT_DEBUG_LEVEL >= 2
#define PRINTF_LOGW     printf("W(%d): ", global_system_time_stamp);printf
#endif
#if PRINT_DEBUG_LEVEL >= 1
#define PRINTF_LOGE     printf("E(%d): ", global_system_time_stamp);printf
#endif
#endif

extern const char compile_date_time[];
extern unsigned int global_system_time_stamp;

void system_config(uint32_t baudrate);
void delayus(uint32_t xus);
void delayms(uint32_t xms);
void usart1_config(uint32_t baudrate);
void print_system_information(void);

#endif 



