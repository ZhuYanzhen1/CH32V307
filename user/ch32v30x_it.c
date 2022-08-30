#include "ch32v30x_it.h"
#include "system.h"

__attribute__((unused)) void NMI_Handler(void) {
    PRINTF_LOGE("\r\nRunning into NMI Handler\r\n");
    while (1);
}
