//
// Created by LaoZhu on 2022/9/3.
//

#include "at24c02.h"
#include "system.h"

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
