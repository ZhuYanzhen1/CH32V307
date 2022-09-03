#include "main.h"

TaskHandle_t task1_handler;

void task1(void *pvParameters) {
    (void) pvParameters;
    while (1) {
        calendar_t calendar;
        rtc_get_time(&calendar);
        PRINTF_LOGI("CPU Usage: %.2f%%    CPU Temperature: %dC\r\n",
                    (float) uTaskGetCPUUsage() / 100.0f, TempSensor_Volt_To_Temper(adc1_get_value(1) * 2500 / 4096))
        PRINTF_LOGI("Get random number: 0x%x\r\n", RNG_GetRandomNumber())
        PRINTF_LOGI("Date Time: %d-%02d-%02d  %02d:%02d:%02d\r\n\r\n", calendar.year, calendar.month, calendar.date,
                    calendar.hour, calendar.min, calendar.sec)
        delayms(1000);
        LED_TOGGLE();

        /* Used to trigger hardware error to test if hardfault back tracking is working */
//        static const uint32_t s_keys[32];
//        uint32_t value =
//            s_keys[((global_system_time_stamp % 10 + 1) & 0x0000001F)] ^ (*(uint32_t *) global_system_time_stamp);
//        printf("value: %d\r\n", value);
    }
}

void user_task_initialize(void) {
    if (xTaskCreate((TaskFunction_t) task1,
                    (const char *) "Task1",
                    (uint16_t) 256,
                    (void *) NULL,
                    (UBaseType_t) 1,
                    (TaskHandle_t *) &task1_handler) != pdPASS) {
        ASSERT_FAILED()
    }
}

void user_hardware_initialize(void) {
    adc1_config();
    adc1_dma1_config();
    adc1_channel_config();
}
