#include "main.h"

TaskHandle_t task1_handler;

void task1(void *pvParameters) {
    (void) pvParameters;
    while (1) {
        printf("CPU Usage: %.2f%%\r\n", (float) uTaskGetCPUUsage() / 100.0f);
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

}
