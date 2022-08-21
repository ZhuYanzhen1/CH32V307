#ifndef __DEBUG_H
#define __DEBUG_H

#include "printf.h"
#include "ch32v30x.h"

extern const char compile_date_time[];

void system_config(uint32_t baudrate);
void delayus(uint32_t xus);
void delayms(uint32_t xms);
void usart1_config(uint32_t baudrate);
void printk_int(const char *str, unsigned int value);
void print_system_information(void);

#endif 



