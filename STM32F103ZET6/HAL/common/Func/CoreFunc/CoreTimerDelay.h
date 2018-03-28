#ifndef __CORE_TIMER_DELAY_H_
#define __CORE_TIMER_DELAY_H_
#include "stm32f1xx.h"

/**系统滴答定时器用作延时Timer */


/*延时初始化*/
void CoreTimerDelayInit(void);

/*延时毫秒*/
void CoreTimerDelayMs(uint32_t nms);

/*延时微秒*/
void CoreTimerDelayFUs(uint16_t nus);


#endif




