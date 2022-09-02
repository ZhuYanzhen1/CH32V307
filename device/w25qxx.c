//
// Created by LaoZhu on 2022/9/3.
//

#include "w25qxx.h"
#include "system.h"
#include "spi.h"

static unsigned char w25qxx_flash_size = 0;
static unsigned char w25qxx_read_sr(void) {
    unsigned char byte;
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_ReadStatusReg);
    byte = spi3_readwrite_byte(0Xff);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    return byte;
}

static void w25qxx_wait_busy(void) {
    while ((w25qxx_read_sr() & 0x01) == 0x01);
}

static void w25qxx_erase_sector(unsigned int addr) {
    addr *= 4096;
    w25qxx_enable_write();
    w25qxx_wait_busy();
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_SectorErase);
    spi3_readwrite_byte((unsigned char) ((addr) >> 16));
    spi3_readwrite_byte((unsigned char) ((addr) >> 8));
    spi3_readwrite_byte((unsigned char) addr);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    w25qxx_wait_busy();
}

static void w25qxx_write_page(unsigned char *data, unsigned int addr, unsigned short size) {
    unsigned short i;
    w25qxx_enable_write();
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_PageProgram);
    spi3_readwrite_byte((unsigned char) ((addr) >> 16));
    spi3_readwrite_byte((unsigned char) ((addr) >> 8));
    spi3_readwrite_byte((unsigned char) addr);
    for (i = 0; i < size; i++)
        spi3_readwrite_byte(data[i]);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    w25qxx_wait_busy();
}

static void w25qxx_write_nocheck(unsigned char *data, unsigned int addr, unsigned short size) {
    unsigned short pageremain;
    pageremain = 256 - addr % 256;
    if (size <= pageremain)
        pageremain = size;
    while (1) {
        w25qxx_write_page(data, addr, pageremain);
        if (size == pageremain)
            break;
        else {
            data += pageremain;
            addr += pageremain;
            size -= pageremain;
            if (size > 256)
                pageremain = 256;
            else
                pageremain = size;
        }
    }
}

void w25qxx_enable_write(void) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_WriteEnable);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

void w25qxx_disable_write(void) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_WriteDisable);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

unsigned short w25qxx_read_id(void) {
    unsigned short Temp = 0;
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_ManufactDeviceID);
    spi3_readwrite_byte(0x00);
    spi3_readwrite_byte(0x00);
    spi3_readwrite_byte(0x00);
    Temp |= spi3_readwrite_byte(0xFF) << 8;
    Temp |= spi3_readwrite_byte(0xFF);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    return Temp;
}

void w25qxx_read_bytes(unsigned int addr, unsigned char *data, unsigned short length) {
    unsigned short i;
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_ReadData);
    spi3_readwrite_byte((unsigned char) ((addr) >> 16));
    spi3_readwrite_byte((unsigned char) ((addr) >> 8));
    spi3_readwrite_byte((unsigned char) addr);
    for (i = 0; i < length; i++)
        data[i] = spi3_readwrite_byte(0XFF);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

static unsigned char w25qxx_flash_buffer[4096];
void w25qxx_write_bytes(unsigned int addr, unsigned char *data, unsigned short length) {
    unsigned int secpos;
    unsigned short secoff, secremain, i;

    secpos = addr / 4096;
    secoff = addr % 4096;
    secremain = 4096 - secoff;

    if (length <= secremain)
        secremain = length;

    while (1) {
        w25qxx_read_bytes(secpos * 4096, w25qxx_flash_buffer, 4096);
        for (i = 0; i < secremain; i++) {
            if (w25qxx_flash_buffer[secoff + i] != 0XFF)
                break;
        }
        if (i < secremain) {
            w25qxx_erase_sector(secpos);
            for (i = 0; i < secremain; i++)
                w25qxx_flash_buffer[i + secoff] = data[i];
            w25qxx_write_nocheck(w25qxx_flash_buffer, secpos * 4096, 4096);
        } else
            w25qxx_write_nocheck(data, addr, secremain);
        if (length == secremain)
            break;
        else {
            secpos++;
            secoff = 0;

            data += secremain;
            addr += secremain;
            length -= secremain;

            if (length > 4096)
                secremain = 4096;
            else
                secremain = length;
        }
    }
}

void w25qxx_erase_chip(void) {
    w25qxx_enable_write();
    w25qxx_wait_busy();
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_ChipErase);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    w25qxx_wait_busy();
}

void w25qxx_power_down(void) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_PowerDown);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    delayus(3);
}

void w25qxx_wakeup(void) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    spi3_readwrite_byte(W25X_ReleasePowerDown);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    delayus(3);
}

static unsigned char w25qxx_data[32] = {0};
void w25qxx_config(void) {
    int counter;
    unsigned short device_id;
    unsigned char tmp_buffer[sizeof(w25qxx_data)] = {0};
    spi3_config();
    w25qxx_wakeup();
    device_id = w25qxx_read_id();
    switch (device_id) {
        case W25Q80:w25qxx_flash_size = 8;
            break;
        case W25Q16:w25qxx_flash_size = 16;
            break;
        case W25Q32:w25qxx_flash_size = 32;
            break;
        case W25Q64:w25qxx_flash_size = 64;
            break;
        case W25Q128:w25qxx_flash_size = 128;
            break;
        default:break;
    }
    if (w25qxx_flash_size != 0) {
        for (counter = 0; counter < sizeof(w25qxx_data); ++counter)
            tmp_buffer[counter] = counter;
        w25qxx_read_bytes((2048 - sizeof(w25qxx_data)) / 2, w25qxx_data, sizeof(w25qxx_data));
        w25qxx_write_bytes((2048 - sizeof(w25qxx_data)) / 2, tmp_buffer, sizeof(w25qxx_data));
        for (counter = 0; counter < sizeof(w25qxx_data); ++counter)
            tmp_buffer[counter] = 0;
        w25qxx_read_bytes((2048 - sizeof(w25qxx_data)) / 2, tmp_buffer, sizeof(w25qxx_data));
        for (counter = 0; counter < sizeof(w25qxx_data); ++counter)
            if (tmp_buffer[counter] != counter)
                break;
        w25qxx_write_bytes((2048 - sizeof(w25qxx_data)) / 2, w25qxx_data, sizeof(w25qxx_data));
        if (counter != sizeof(w25qxx_data)) {
            PRINTF_LOGW("W25Q%d read write test failed, data:", w25qxx_flash_size == 8 ? 80 : w25qxx_flash_size)
#if PRINT_DEBUG_LEVEL >= 2
            printf(LOG_COLOR_W);
            for (counter = 0; counter < sizeof(w25qxx_data); ++counter) {
                if (counter % 8 == 0)
                    printf("\r\n\t");
                printf("0x%02x ", tmp_buffer[counter]);
            }
            printf("\r\n\r\n");
            printf(LOG_RESET_COLOR);
#endif
            w25qxx_flash_size = 0;
        } else PRINTF_LOGI("W25Q%d read write test success\r\n", w25qxx_flash_size == 8 ? 80 : w25qxx_flash_size)
    } else PRINTF_LOGW("Unknown flash type, SPI flash configuration failed\r\n")
}
