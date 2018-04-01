#ifndef __CORE_DAC_H_
#define __CORE_DAC_H_
#include "stm32f10x.h"

/**DAC初始化 */
void CoreDACInit(void);

/**DAC设置参数值 */
void CoreDACSetValue(uint32_t channel, uint16_t vol);

#endif




