#include "main.h"

TaskHandle_t task1_handler;

void task1(void *pvParameters) {
    (void) pvParameters;
    while (1) {
        PRINTF_LOGI("CPU Usage: %.2f%%    CPU Temperature: %dC\r\n",
                    (float) uTaskGetCPUUsage() / 100.0f, TempSensor_Volt_To_Temper(adc1_get_value(1) * 3300 / 4096));
        delayms(1000);
    }
}

void user_task_initialize(void) {
    if (xTaskCreate((TaskFunction_t) task1,
                    (const char *) "Task1",
                    (uint16_t) 256,
                    (void *) NULL,
                    (UBaseType_t) 1,
                    (TaskHandle_t *) &task1_handler) != pdPASS) {
        ASSERT_FAILED();
    }
}

void user_hardware_initialize(void) {
    adc1_config();
    adc1_dma1_config();
    adc1_channel_config();
}
