#include "ch32v30x_it.h"
#include "system.h"

__attribute__((unused)) void NMI_Handler(void) {
    PRINTF_LOGE("\r\nRunning into NMI Handler\r\n");
    while (1);
}

__attribute__((unused)) void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        while ((USART1->STATR & USART_FLAG_TC) == RESET);
        USART1->DATAR = (USART_ReceiveData(USART1) & (uint16_t) 0x01FF);
    }
}
