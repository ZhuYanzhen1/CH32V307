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

void system_config(uint32_t baudrate) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    unsigned short tmpcr1 = TIM6->CTLR1;
    tmpcr1 &= (uint16_t) (~((uint16_t) TIM_CTLR1_CKD));
    tmpcr1 |= (uint32_t) TIM_CKD_DIV1;
    TIM6->CTLR1 = tmpcr1 | 0x80;
    TIM6->ATRLR = 0xffff;
    TIM6->PSC = 0;
    TIM6->DMAINTENR |= TIM_IT_Update;
    TIM6->SWEVGR = TIM_PSCReloadMode_Immediate;
    usart1_config(baudrate);
    print_system_information();
}

void delayus(uint32_t xus) {
    TIM6->CNT = 0;
    TIM6->SWEVGR = TIM_PSCReloadMode_Immediate;
    TIM6->CTLR1 |= TIM_CEN;
    for (unsigned int counter = 0; counter < xus; ++counter) {
        while (TIM6->CNT <= (144 - 1));
        TIM6->CNT = 0;
        TIM6->SWEVGR = TIM_PSCReloadMode_Immediate;
    }
    TIM6->CTLR1 &= (uint16_t) (~((uint16_t) TIM_CEN));
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

static const char hex_to_str_table[] = "0123456789ABCDEF";
void printk_int(const char *str, unsigned int value) {
    while (1) {
        if (*str != '\0') {
            while ((USART1->STATR & USART_FLAG_TC) == RESET);
            USART1->DATAR = (*str & (uint16_t) 0x01FF);
            str++;
        } else
            break;
    }
    while ((USART1->STATR & USART_FLAG_TC) == RESET);
    USART1->DATAR = ('0' & (uint16_t) 0x01FF);
    while ((USART1->STATR & USART_FLAG_TC) == RESET);
    USART1->DATAR = ('x' & (uint16_t) 0x01FF);
    for (unsigned char counter = 0; counter < 8; ++counter) {
        while ((USART1->STATR & USART_FLAG_TC) == RESET);
        USART1->DATAR = (hex_to_str_table[(value >> (4 * counter)) & 0x0F] & (uint16_t) 0x01FF);
    }
}

void _putchar(char character) {
    while ((USART1->STATR & USART_FLAG_TC) == RESET);
    USART1->DATAR = (character & (uint16_t) 0x01FF);
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
