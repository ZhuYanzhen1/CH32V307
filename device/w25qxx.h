//
// Created by LaoZhu on 2022/9/3.
//

#ifndef CH32V307VC_DEVICE_W25QXX_H_
#define CH32V307VC_DEVICE_W25QXX_H_

/* Winbond SPIFalsh ID */
#define W25Q80                   0XEF13
#define W25Q16                   0XEF14
#define W25Q32                   0XEF15
#define W25Q64                   0XEF16
#define W25Q128                  0XEF17

/* Winbond SPIFalsh Instruction List */
#define W25X_WriteEnable         0x06
#define W25X_WriteDisable        0x04
#define W25X_ReadStatusReg       0x05
#define W25X_ReadData            0x03
#define W25X_PageProgram         0x02
#define W25X_SectorErase         0x20
#define W25X_ChipErase           0xC7
#define W25X_PowerDown           0xB9
#define W25X_ReleasePowerDown    0xAB
#define W25X_ManufactDeviceID    0x90

void w25qxx_read_bytes(unsigned int addr, unsigned char *data, unsigned short length);
void w25qxx_write_bytes(unsigned int addr, unsigned char *data, unsigned short length);
void w25qxx_erase_chip(void);
void w25qxx_power_down(void);
void w25qxx_wakeup(void);
void w25qxx_enable_write(void);
void w25qxx_disable_write(void);
unsigned short w25qxx_read_id(void);
unsigned int w25qxx_get_size(void);
void w25qxx_config(void);

#endif //CH32V307VC_DEVICE_W25QXX_H_
