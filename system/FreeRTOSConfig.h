#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "system.h"
#include "config.h"

/* don't have MTIME */
#define configMTIME_BASE_ADDRESS            ( 0 )
#define configMTIMECMP_BASE_ADDRESS         ( 0 )

#define configUSE_CPU_USAGE_CALCULATE       1
#ifdef configUSE_CPU_USAGE_CALCULATE
#define configCPU_USAGE_CALC_PERIOD         1000
#endif

#define configUSE_PREEMPTION                1
#define configUSE_IDLE_HOOK                 0
#define configUSE_TICK_HOOK                 0
#define configCPU_CLOCK_HZ                  SystemCoreClock
#define configTICK_RATE_HZ                  ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                ( 15 )
#define configMINIMAL_STACK_SIZE            ( ( unsigned short ) 256 )
#define configTOTAL_HEAP_SIZE               ( ( size_t ) ( FREERTOS_HEAP_SIZE ) )
#define configMAX_TASK_NAME_LEN             ( 16 )
#define configUSE_TRACE_FACILITY            0
#define configUSE_16_BIT_TICKS              0
#define configIDLE_SHOULD_YIELD             0
#define configUSE_MUTEXES                   1
#define configQUEUE_REGISTRY_SIZE           8
#define configCHECK_FOR_STACK_OVERFLOW      0
#define configUSE_RECURSIVE_MUTEXES         1
#define configUSE_MALLOC_FAILED_HOOK        0
#define configUSE_APPLICATION_TASK_TAG      0
#define configUSE_COUNTING_SEMAPHORES       1
#define configGENERATE_RUN_TIME_STATS       0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES               0
#define configMAX_CO_ROUTINE_PRIORITIES     ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH            4
#define configTIMER_TASK_STACK_DEPTH        ( configMINIMAL_STACK_SIZE )



/* Set the following definitions to 1 to include the API function, or zero
   to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xSemaphoreGetMutexHolder        1


/* Normal assert() semantics without relying on the provision of an assert.h
   header file. */
#if (PRINT_DEBUG_INFORMATION == 1)
#define configASSERT(x) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); printf("Error in file \"%s\" %d. \r\n ", __FILE__, __LINE__); while(1); }
#else
#define configASSERT(x) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); while(1); }
#endif

/* Map to the platform printf function. */
#define configPRINT_STRING(pcString)  printf( pcString )

#endif /* FREERTOS_CONFIG_H */
