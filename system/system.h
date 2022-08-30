#ifndef __DEBUG_H
#define __DEBUG_H

#include "printf.h"
#include "ch32v30x.h"
#include "config.h"
#include "gitver.h"

#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)

#if PRINT_DEBUG_LEVEL >= 3
#define PRINTF_LOGI(format, ...)    { printf(LOG_COLOR_I); printf("I(%d): ", global_system_time_stamp / 10); \
                                    printf(format, ##__VA_ARGS__); printf(LOG_RESET_COLOR); }
#else
#define PRINTF_LOGI(format, ...)
#endif
#if PRINT_DEBUG_LEVEL >= 2
#define PRINTF_LOGW(format, ...)    { printf(LOG_COLOR_W); printf("W(%d): ", global_system_time_stamp / 10); \
                                    printf(format, ##__VA_ARGS__); printf(LOG_RESET_COLOR); }
#else
#define PRINTF_LOGW(format, ...)
#endif
#if PRINT_DEBUG_LEVEL >= 1
#define PRINTF_LOGE(format, ...)    { printf(LOG_COLOR_E); printf("E(%d): ", global_system_time_stamp / 10); \
                                    printf(format, ##__VA_ARGS__); printf(LOG_RESET_COLOR); }
#define ASSERT_FAILED()              { PRINTF_LOGE("Initialize or create failed in \"%s\" %d.\r\n", __FILE__, __LINE__); while (1); }
#else
#define PRINTF_LOGE(format, ...)
#define ASSERT_FAILED()              while(1)
#endif

extern const char compile_date_time[];
extern unsigned int global_system_time_stamp;

void delayus(uint32_t xus);
void delayms(uint32_t xms);

#endif 



