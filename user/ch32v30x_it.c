#include "ch32v30x_it.h"

__attribute__((unused)) void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
__attribute__((unused)) void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

__attribute__((unused)) void NMI_Handler(void) {
    unsigned int mepc, mcause, mtval;
    __asm volatile ( "csrr %0," "mepc" : "=r" (mepc));
    __asm volatile ( "csrr %0," "mcause" : "=r" (mcause));
    __asm volatile ( "csrr %0," "mtval" : "=r" (mtval));
    __asm volatile ("csrw 0x800, %0" : : "r" (0x6000));
    printk_int("\r\nHardFault_Handler\r\nmpec: ", mepc);
    printk_int("  mcause: ", mcause);
    printk_int("  mtval: ", mtval);
    while (1);
}

__attribute__((unused)) void HardFault_Handler(void) {
    unsigned int mepc, mcause, mtval;
    __asm volatile ( "csrr %0," "mepc" : "=r" (mepc));
    __asm volatile ( "csrr %0," "mcause" : "=r" (mcause));
    __asm volatile ( "csrr %0," "mtval" : "=r" (mtval));
    __asm volatile ("csrw 0x800, %0" : : "r" (0x6000));
    printk_int("\r\nHardFault_Handler\r\nmpec: ", mepc);
    printk_int("  mcause: ", mcause);
    printk_int("  mtval: ", mtval);
    while (1);
}
