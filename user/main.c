#include "main.h"

TaskHandle_t task1_handler;

void task1(void *pvParameters) {
    (void) pvParameters;
     unsigned char buffer[4 * 1024] = {0};
    while (1) {
        buffer[(global_system_time_stamp * 97) % (4 * 1024)] =
            TempSensor_Volt_To_Temper(adc1_get_value(1) * 3300 / 4096);
        printf("Temperature: %d\r\n", buffer[global_system_time_stamp % (4 * 1024) + 1]);
        printf("CPU Usage: %.2f%%\r\n\r\n", (float) uTaskGetCPUUsage() / 100.0f);
        delayms(1000);
    }
}

void user_task_initialize(void) {
    xTaskCreate((TaskFunction_t) task1,
                (const char *) "Task1",
                (uint16_t) 256,
                (void *) NULL,
                (UBaseType_t) 1,
                (TaskHandle_t *) &task1_handler);
}

void user_hardware_initialize(void) {
    adc1_config();
    adc1_dma1_config();
    adc1_channel_config();
}
