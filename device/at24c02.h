//
// Created by LaoZhu on 2022/9/3.
//

#ifndef CH32V307VC_DEVICE_AT24C02_H_
#define CH32V307VC_DEVICE_AT24C02_H_

void at24c02_read_bytes(unsigned short addr, unsigned char *data, unsigned short length);
void at24c02_write_bytes(unsigned short addr, unsigned char *data, unsigned short length);
void at24c02_config(void);

#endif //CH32V307VC_DEVICE_AT24C02_H_
