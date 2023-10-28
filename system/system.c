#include "system.h"
#include "main.h"

static TaskHandle_t initialize_task_handler;

__attribute__((unused)) void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
__attribute__((unused)) void HardFault_Handler(void) {
    unsigned int mepc, mcause, mtval;
    __asm volatile ( "csrr %0," "mepc" : "=r" (mepc));
    __asm volatile ( "csrr %0," "mcause" : "=r" (mcause));
    __asm volatile ( "csrr %0," "mtval" : "=r" (mtval));
    __asm volatile ("csrw 0x800, %0" : : "r" (0x6000));
#if (PRINT_DEBUG_LEVEL != 0)
    printf("\r\n");
#endif
    PRINTF_LOGE("Running into HardFault Handler\r\n")
#if (PRINT_DEBUG_LEVEL != 0)
    printf(LOG_COLOR_E);
    printf("E(%d): Abnormality reason: ", global_system_time_stamp / 10);
    if ((mcause & 0x80000000UL) != 0) {
        printf("Unknown");
    } else {
        switch (mcause & 0x0000000FUL) {
            case 0: printf("Instruction address misalignment%s\r\n", LOG_RESET_COLOR);
                break;
            case 1: printf("Fetch command access error%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("Address of memory access: 0x%08x\r\n", mtval)
                break;
            case 2: printf("Illegal instructions%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("Illegal instruction code: 0x%08x\r\n", mtval)
                break;
            case 3: printf("Breakpoints%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("PC Register value: 0x%08x\r\n", mtval)
                break;
            case 4: printf("Load instruction access address misalignment%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("Address of memory access: 0x%08x\r\n", mtval)
                break;
            case 5: printf("Load command access error%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("Address of memory access: 0x%08x\r\n", mtval)
                break;
            case 6: printf("Store/AMO instruction access address misalignment%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("Address of memory access: 0x%08x\r\n", mtval)
                break;
            case 7: printf("Store/AMO command access error%s\r\n", LOG_RESET_COLOR);
                PRINTF_LOGE("Address of memory access: 0x%08x\r\n", mtval)
                break;
            default: printf("Unknown%s\r\n", LOG_RESET_COLOR);
                break;
        }
    }
#endif
    PRINTF_LOGE("Current Task: %s, dumping register data\r\n", pcTaskGetName(xTaskGetCurrentTaskHandle()))
    PRINTF_LOGE("mpec: 0x%08X    mcause: 0x%08X    mtval: 0x%08X\r\n\r\n", mepc, mcause, mtval)
    PRINTF_LOGE("Run command to find error line:\r\n")
    PRINTF_LOGE("%saddr2line.exe -e %s -f 0x%x -a -p\r\n", TOOLCHAIN_PATH, PROJECT_ELF_PATH, mepc)
    while (1)
        IWDG_ReloadCounter();
}

#if (PRINT_DEBUG_LEVEL != 0)
static char stackoverflow_sprintf_buffer[64] = {0};
static void usart1_sendstring(char *str) {
    _putchar(*str);
    while (*++str)
        _putchar(*str);
}
#endif
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
#if (PRINT_DEBUG_LEVEL != 0)
    sprintf(stackoverflow_sprintf_buffer, "%sE(%d): %s task stack overflow%s\r\n",
            LOG_COLOR_E, global_system_time_stamp / 10, pcTaskName, LOG_RESET_COLOR);
    usart1_sendstring(stackoverflow_sprintf_buffer);
    sprintf(stackoverflow_sprintf_buffer, "%sE(%d): FreeRTOS free heap size: %dB%s\r\n",
            LOG_COLOR_E, global_system_time_stamp / 10, xPortGetFreeHeapSize(), LOG_RESET_COLOR);
    usart1_sendstring(stackoverflow_sprintf_buffer);
#endif
    while (1);
}

static void system_rng_bkp_config() {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG | RCC_AHBPeriph_CRC, ENABLE);
    RNG_Cmd(ENABLE);
    while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    BKP_TamperPinCmd(DISABLE);
    PWR_BackupAccessCmd(ENABLE);
    BKP_ClearFlag();
}

static TimerHandle_t iwdg_feed_timer;
static void iwdg_feed_timer_callback(TimerHandle_t xTimer) {
    IWDG_ReloadCounter();
}
static void create_iwdg_task(void) {
    if ((BKP_ReadBackupRegister(BKP_DR1) & 0x00FF) <= 3) {
        iwdg_feed_timer = xTimerCreate("IWDG_Feed", 2000, pdTRUE,
                                       (void *) 0, iwdg_feed_timer_callback);
        if (iwdg_feed_timer != NULL) {
            if (xTimerStart(iwdg_feed_timer, 0x000000FFUL) == pdPASS) {
                IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
                IWDG_SetPrescaler(IWDG_Prescaler_32);
                IWDG_SetReload(4000);
                IWDG_ReloadCounter();
                IWDG_Enable();
            } else PRINTF_LOGW("IWDG timer start failed, disable IWDG\r\n")
        } else PRINTF_LOGW("IWDG timer alloc memory failed, disable IWDG\r\n")
    } else {
        PRINTF_LOGE("The watchdog has been reset more than three times in a row.\r\n")
        PRINTF_LOGE("Please check the previously printed logs to troubleshoot error messages.\r\n")
        while (1);
    }
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
#if (PRINT_DEBUG_LEVEL == 3)
    printf("%s------------------------ System Configuration ------------------------%s\r\n",
           LOG_COLOR_I,
           LOG_RESET_COLOR);
#endif
    system_rng_bkp_config();
    led_config();
    key_config();
    at24c02_config();
    w25qxx_config();
    rtc_config();
    fs_config();
#if (PRINT_DEBUG_LEVEL == 3)
    printf("\r\n%s------------------------- User Configuration -------------------------%s\r\n",
           LOG_COLOR_I,
           LOG_RESET_COLOR);
#endif
    user_hardware_initialize();
    taskEXIT_CRITICAL();
    printf_semaphore = xSemaphoreCreateMutex();
    user_task_initialize();
    create_iwdg_task();
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
    portDISABLE_INTERRUPTS();
    SystemCoreClockUpdate();
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
#if (PRINT_DEBUG_LEVEL == 3)
    unsigned int rst_reason = RCC->RSTSCKR;
    unsigned int chip_uid1 = *((volatile unsigned int *) 0x1FFFF7E8UL);
    unsigned int chip_uid2 = *((volatile unsigned int *) 0x1FFFF7ECUL);
    unsigned int opt_byte_reg = *((volatile unsigned int *) 0x4002201CUL);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
#endif
#if (STARTUP_CLEAR_SCREEN == 1)
    delayms(700);
    printf("\033c");
#endif
#if (PRINT_DEBUG_LEVEL == 3)
    printf("%s------------------------- System Information -------------------------\r\n", LOG_COLOR_I);
    unsigned int misa_value = __get_MISA();
    printf("Instruction set: RV32");
    for (unsigned char counter = 0; counter < 25; ++counter)
        if (((misa_value >> counter) & 0x00000001UL) == 1)
            _putchar((char) (65 + counter));
    printf("\tReset reason: ");
    if (((rst_reason << 4) & 0x80000000UL) == 0) {
        for (unsigned char counter = 0; counter < 6; ++counter) {
            if (((rst_reason << counter) & 0x80000000UL) != 0) {
                switch (counter) {
                    case 0:printf("Low power ");
                        break;
                    case 1:printf("Window watchdog ");
                        BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1) + 1);
                        break;
                    case 2:printf("Independent watchdog ");
                        BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1) + 1);
                        break;
                    case 3:printf("Software ");
                        BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1) & 0xFF00);
                        break;
                    case 4:printf("Power-on ");
                        break;
                    case 5:printf("External ");
                        BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1) & 0xFF00);
                        break;
                    default:printf("Unknown ");
                        break;
                }
            }
        }
        printf("reset");
    } else {
        printf("Power-on reset");
        BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1) & 0xFF00);
    }
    RCC->RSTSCKR = RCC->RSTSCKR | 0x01000000UL;
    printf("\r\nSystem clock frequency: %dMHz", (SystemCoreClock / 1000000));
    printf("\tFreeRTOS kernel version: %s\r\n", tskKERNEL_VERSION_NUMBER);
    printf("Program git version: %s", GIT_COMMIT_HASH);
    printf("\tCompiled time: %s\r\n", FIRMWARE_BUILD_TIME);
    printf("Compiled toolchain: GCC %s\r\n", GCC_VERSION_MAJOR);
    printf("SRAM mode: ");
    switch ((opt_byte_reg & 0x00000300UL) >> 8) {
        case 0:printf("192KB + 128KB");
            break;
        case 1:printf("224KB + 96KB");
            break;
        case 2:printf("256KB + 64KB");
            break;
        case 3:printf("288KB + 32KB");
            break;
    }
    printf("\t\tUnique ID: %08X%08X\r\n", chip_uid1, chip_uid2);
    printf("%s\r\n", LOG_RESET_COLOR);
#endif
}
