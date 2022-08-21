/********************************** (C) COPYRIGHT  *******************************
* File Name          : debug.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for UART
*                      Printf , Delay functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#include "printf.h"
#include "ch32v30x.h"

extern const char compile_date_time[];

void system_config(uint32_t baudrate);
void delayus (uint32_t xus);
void delayms (uint32_t xms);
void usart1_config(uint32_t baudrate);
void print_system_information(void);

#endif 



