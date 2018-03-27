#ifndef __CORE_TICK_DELAY_H_
#define __CORE_TICK_DELAY_H_
#include "stm32f4xx.h"

/**系统延时函数初始化 */
void CoreTickDelayInit(void);

/**系统延时毫秒 */
void CoreTickDelayMs(uint32_t nms);

/**系统延时微秒 */
void CoreTickDelayUs(uint16_t nus);


#endif






