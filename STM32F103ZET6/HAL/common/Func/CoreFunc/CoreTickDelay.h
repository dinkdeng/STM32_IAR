#ifndef __CORE_TICK_DELAY_H_
#define __CORE_TICK_DELAY_H_
#include "stm32f1xx_hal.h"

/**系统滴答定时器用作延时 */


/*延时初始化*/
void CoreTickDelayInit(void);

/*延时毫秒*/
void CoreTickDelayMs(uint32_t nms);

/*延时微秒*/
void CoreTickDelayFUs(uint16_t nus);


#endif




