#include "FreeRTOS.h"
#include "task.h"
#include "system.h"
#include "semphr.h"

const char compile_date_time[] = __TIMESTAMP__;

static TaskHandle_t initialize_task_handler;

extern void user_hardware_initialize(void);
extern void user_task_initialize(void);
void initialize_task(void *pvParameters) {
    (void) pvParameters;
    extern SemaphoreHandle_t printf_semaphore;
    printf_semaphore = xSemaphoreCreateMutex();
#if (configUSE_CPU_USAGE_CALCULATE == 1)
    vTaskCPUUsageInit();
#endif
    taskENTER_CRITICAL();
    user_hardware_initialize();
    taskEXIT_CRITICAL();
    user_task_initialize();
    vTaskDelete(NULL);
}

int main(void) {
    system_config(115200);
    xTaskCreate((TaskFunction_t) initialize_task,
                (const char *) "InitTask",
                (uint16_t) 1024,
                (void *) NULL,
                (UBaseType_t) 2,
                (TaskHandle_t *) &initialize_task_handler);
    vTaskStartScheduler();
    while (1);
}

static uint8_t factor_us = 0;
void system_config(uint32_t baudrate) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    factor_us = SystemCoreClock / 8000000;
    usart1_config(baudrate);
    print_system_information();
}

void delayus(uint32_t xus) {
    uint32_t i;
    SysTick->SR &= ~(1 << 0);
    i = (uint32_t)xus * factor_us;
    SysTick->CMP = i;
    SysTick->CTLR |= (1 << 4) | (1 << 5) | (1 << 0);
    while((SysTick->SR & (1 << 0)) != (1 << 0));
    SysTick->CTLR &= ~(1 << 0);
}

extern UBaseType_t uxCriticalNesting;
void delayms(uint32_t xms) {
    if (uxCriticalNesting == 0)
        vTaskDelay(xms);
    else
        delayus(xms * 1000);
}

void usart1_config(uint32_t baudrate) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void _putchar(char character){
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, character);
}

void *_sbrk(ptrdiff_t incr) {
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;
    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
        return NULL - 1;
    curbrk += incr;
    return curbrk - incr;
}

void print_system_information(void) {
#if (PRINT_DEBUG_INFORMATION == 1)
    printf("--------------------- System Information ---------------------\r\n\r\n");
    unsigned int misa_value = __get_MISA();
    printf("Instruction set: RV32");
    for (unsigned char counter = 0; counter < 25; ++counter)
        if (((misa_value >> counter) & 0x00000001UL) == 1)
            _putchar((char)(65 + counter));
    printf("\r\nSystem clock frequency: %dMHz\r\n", (SystemCoreClock / 1000000));
    printf("FreeRTOS kernel version: %s\r\n", tskKERNEL_VERSION_NUMBER);
    printf("Firmware compiled in %s\r\n\r\n", compile_date_time);
    printf("-------------------------------------------------------------\r\n\r\n");
#endif
}