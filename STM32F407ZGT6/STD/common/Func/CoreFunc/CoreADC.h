#ifndef __CORE_ADC_H_
#define __CORE_ADC_H_
#include "stm32f4xx.h"

/**初始化 */
void CoreADC_Init(void);

/**获取指定通道的ADC的值 */
uint16_t CoreADC_GetValueBychannel(uint32_t channel,uint16_t times);



#endif







