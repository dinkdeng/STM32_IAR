#ifndef __CORE_DAC_H_
#define __CORE_DAC_H_
#include "stm32f4xx.h"

/**DAC初始化 */
void CoreDAC_Init(void);

/**DAC设置参数值 */
void CoreDAC_SetValue(uint16_t vol);

#endif




