//
// Created by LaoZhu on 2022/8/22.
//

#ifndef CH32V307VC_HARDWARE_ADC_H_
#define CH32V307VC_HARDWARE_ADC_H_

unsigned short adc1_get_value(unsigned char index);
void adc1_config(void);
void adc1_dma1_config(void);
void adc1_channel_config(void);

#endif //CH32V307VC_HARDWARE_ADC_H_
