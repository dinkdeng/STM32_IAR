#ifndef __CORE_ADC_H_
#define __CORE_ADC_H_
#include "stm32f10x.h"

/**初始化 */
void CoreADCInit(void);

/**获取指定通道的ADC的值 */
uint16_t CoreADCGetValueBychannel(uint32_t channel,uint16_t times);



#endif







