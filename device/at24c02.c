//
// Created by LaoZhu on 2022/9/3.
//

#include "at24c02.h"
#include "system.h"
#include "iic.h"

static unsigned char at24c02_read_byte(unsigned short ReadAddr) {
    unsigned char temp;

    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(I2C2, ENABLE);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C2, 0XA0, I2C_Direction_Transmitter);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, (unsigned char) (ReadAddr & 0x00FF));
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C2, ENABLE);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C2, 0XA0, I2C_Direction_Receiver);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
        I2C_AcknowledgeConfig(I2C2, DISABLE);

    temp = I2C_ReceiveData(I2C2);
    I2C_GenerateSTOP(I2C2, ENABLE);

    return temp;
}

static void at24c02_write_byte(unsigned short WriteAddr, unsigned char DataToWrite) {
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(I2C2, ENABLE);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C2, 0XA0, I2C_Direction_Transmitter);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, (unsigned char) (WriteAddr & 0x00FF));
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    if (I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) != RESET)
        I2C_SendData(I2C2, DataToWrite);

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C2, ENABLE);
}

void at24c02_read_bytes(unsigned short addr, unsigned char *data, unsigned short length) {
    while (length) {
        *data++ = at24c02_read_byte(addr++);
        length--;
    }
}

void at24c02_write_bytes(unsigned short addr, unsigned char *data, unsigned short length) {
    while (length--) {
        at24c02_write_byte(addr, *data);
        addr++;
        data++;
        delayms(2);
    }
}

static unsigned char at24c02_data[32] = {0};
void at24c02_config(void) {
    int counter;
    unsigned char tmp_buffer[sizeof(at24c02_data)] = {0};
    for (counter = 0; counter < sizeof(at24c02_data); ++counter)
        tmp_buffer[counter] = counter;
    iic2_config(400000, 0xA0);
    at24c02_read_bytes((256 - sizeof(at24c02_data)) / 2, at24c02_data, sizeof(at24c02_data));
    at24c02_write_bytes((256 - sizeof(at24c02_data)) / 2, tmp_buffer, sizeof(at24c02_data));
    for (counter = 0; counter < sizeof(at24c02_data); ++counter)
        tmp_buffer[counter] = 0;
    at24c02_read_bytes((256 - sizeof(at24c02_data)) / 2, tmp_buffer, sizeof(at24c02_data));
    for (counter = 0; counter < sizeof(at24c02_data); ++counter)
        if (tmp_buffer[counter] != counter)
            break;
    if (counter != sizeof(at24c02_data)) {
        PRINTF_LOGW("AT24C02 read write test failed, data:")
#if PRINT_DEBUG_LEVEL >= 2
        printf(LOG_COLOR_W);
        for (counter = 0; counter < sizeof(at24c02_data); ++counter) {
            if (counter % 8 == 0)
                printf("\r\n\t");
            printf("0x%02x ", tmp_buffer[counter]);
        }
        printf("\r\n\r\n");
        printf(LOG_RESET_COLOR);
#endif
    } else PRINTF_LOGI("AT24C02 read write test success\r\n")
    at24c02_write_bytes((256 - sizeof(at24c02_data)) / 2, at24c02_data, sizeof(at24c02_data));
}
