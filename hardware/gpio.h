//
// Created by LaoZhu on 2022/9/3.
//

#ifndef CH32V307VC_HARDWARE_GPIO_H_
#define CH32V307VC_HARDWARE_GPIO_H_

#define LED_ON()        GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET)
#define LED_OFF()       GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET)
#define LED_TOGGLE()    GPIO_WriteBit(GPIOE, GPIO_Pin_2, !GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_2))

void led_config(void);
void key_config(void);

#endif //CH32V307VC_HARDWARE_GPIO_H_
