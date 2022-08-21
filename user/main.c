#include "main.h"

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;

void task2(void *pvParameters) {
    (void) pvParameters;
    float b = 1;
    while(1) {
        b = b + 0.2f;
        printf("task2 entry, b = %f\r\n\r\n", b);
        vTaskDelay(500);
    }
}

void task1(void *pvParameters) {
    (void) pvParameters;
    float a = 0;
    while (1) {
        a = a + 0.1f;
        printf("task1, CPU Usage: %f\r\n", (float)uTaskGetCPUUsage() / 100.0f);
        printf("task1 entry, a = %f\r\n\r\n", a);
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
    xTaskCreate((TaskFunction_t) task2,
                (const char *) "Task2",
                (uint16_t) 256,
                (void *) NULL,
                (UBaseType_t) 1,
                (TaskHandle_t *) &task2_handler);
}

void user_hardware_initialize(void) {

}
