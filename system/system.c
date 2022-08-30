#include "FreeRTOS.h"
#include "task.h"
#include "system.h"
#include "semphr.h"
#include "usart.h"

const char compile_date_time[] = __TIMESTAMP__;

static TaskHandle_t initialize_task_handler;

__attribute__((unused)) void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
__attribute__((unused)) void HardFault_Handler(void) {
    unsigned int mepc, mcause, mtval;
    __asm volatile ( "csrr %0," "mepc" : "=r" (mepc));
    __asm volatile ( "csrr %0," "mcause" : "=r" (mcause));
    __asm volatile ( "csrr %0," "mtval" : "=r" (mtval));
    __asm volatile ("csrw 0x800, %0" : : "r" (0x6000));
    printf("\r\n");
    PRINTF_LOGE("Running into HardFault Handler\r\n");
    PRINTF_LOGE("mpec: 0x%08X    mcause: 0x%08X    mtval: 0x%08X\r\n", mepc, mcause, mtval);
    PRINTF_LOGE("Current Task: %s\r\n\r\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    PRINTF_LOGW("Run command to find error line:\r\n");
    PRINTF_LOGW("%saddr2line.exe -e %s -f 0x%x -a -p\r\n", TOOLCHAIN_PATH, PROJECT_PATH, mepc);
    while (1);
}

static char stackoverflow_sprintf_buffer[64] = {0};
static void usart1_sendstring(char *str) {
    _putchar(*str);
    while (*++str)
        _putchar(*str);
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    sprintf(stackoverflow_sprintf_buffer, "%sE(%d): %s task stack overflow%s\r\n",
            LOG_COLOR_E, global_system_time_stamp / 10, pcTaskName, LOG_RESET_COLOR);
    usart1_sendstring(stackoverflow_sprintf_buffer);
    sprintf(stackoverflow_sprintf_buffer, "%sE(%d): FreeRTOS free heap size: %dB%s\r\n",
            LOG_COLOR_E, global_system_time_stamp / 10, xPortGetFreeHeapSize(), LOG_RESET_COLOR);
    usart1_sendstring(stackoverflow_sprintf_buffer);
    while (1);
}

static void system_rng_bkp_config() {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
    while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    BKP_TamperPinCmd(DISABLE);
    PWR_BackupAccessCmd(ENABLE);
    BKP_ClearFlag();
}

extern void user_hardware_initialize(void);
extern void user_task_initialize(void);
void initialize_task(void *pvParameters) {
    (void) pvParameters;
    extern SemaphoreHandle_t printf_semaphore;
#if (configUSE_CPU_USAGE_CALCULATE == 1)
    vTaskCPUUsageInit();
#endif
    taskENTER_CRITICAL();
    system_rng_bkp_config();
    user_hardware_initialize();
    taskEXIT_CRITICAL();
    printf_semaphore = xSemaphoreCreateMutex();
    user_task_initialize();
    vTaskDelete(NULL);
}

void print_system_information(void);
static void system_timer6_config() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    unsigned short tmpcr1 = TIM6->CTLR1;
    tmpcr1 &= (uint16_t) (~((uint16_t) TIM_CTLR1_CKD));
    tmpcr1 |= (uint32_t) TIM_CKD_DIV1;
    TIM6->CTLR1 = tmpcr1 | 0x80;
    TIM6->ATRLR = 0xffff;
    TIM6->PSC = 0;
    TIM6->DMAINTENR |= TIM_IT_Update;
    TIM6->SWEVGR = TIM_PSCReloadMode_Immediate;
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    system_timer6_config();
    usart1_config(DEBUG_SERIAL_BAUDRATE);
    print_system_information();
    xTaskCreate((TaskFunction_t) initialize_task,
                (const char *) "InitTask",
                (uint16_t) 1024,
                (void *) NULL,
                (UBaseType_t) 2,
                (TaskHandle_t *) &initialize_task_handler);
    vTaskStartScheduler();
    while (1);
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
    unsigned int rst_reason = RCC->RSTSCKR;
    delayms(700);
    printf("\033c");
#if (PRINT_DEBUG_LEVEL == 3)
    printf("%s--------------------- System Information ---------------------\r\n", LOG_COLOR_I);
    unsigned int misa_value = __get_MISA();
    printf("Instruction set: RV32");
    for (unsigned char counter = 0; counter < 25; ++counter)
        if (((misa_value >> counter) & 0x00000001UL) == 1)
            _putchar((char) (65 + counter));
    printf("\r\nReset Reason: ");
    if (((rst_reason << 4) & 0x80000000UL) == 0) {
        for (unsigned char counter = 0; counter < 6; ++counter) {
            if (((rst_reason << counter) & 0x80000000UL) != 0) {
                switch (counter) {
                    case 0:printf("Low power reset");
                        break;
                    case 1:printf("Window watchdog reset");
                        break;
                    case 2:printf("Independent watchdog reset");
                        break;
                    case 3:printf("Software reset");
                        break;
                    case 4:printf("Power-on reset");
                        break;
                    case 5:printf("External reset");
                        break;
                    default:printf("Unknown reset");
                        break;
                }
            }
        }
    } else
        printf("Power-on reset");
    RCC->RSTSCKR = RCC->RSTSCKR | 0x01000000UL;
    printf("\r\nSystem clock frequency: %dMHz\r\n", (SystemCoreClock / 1000000));
    printf("FreeRTOS kernel version: %s\r\n", tskKERNEL_VERSION_NUMBER);
    printf("Firmware compiled in %s\r\n", compile_date_time);
    printf("-------------------------------------------------------------%s\r\n\r\n", LOG_RESET_COLOR);
#endif
}
