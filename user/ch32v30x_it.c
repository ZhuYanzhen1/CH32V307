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

__attribute__((unused)) void RTC_IRQHandler(void) {
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_ITConfig(RTC_IT_ALR, DISABLE);
        PRINTF_LOGI("Alarm time has reached!\r\n")
    }
    RTC_WaitForLastTask();
}

__attribute__((unused)) void EXTI3_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
        PRINTF_LOGI("Key had been pushed!\r\n")
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}
