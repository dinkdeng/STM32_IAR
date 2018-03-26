#ifndef __CORE_TICK_DELAY_H_
#define __CORE_TICK_DELAY_H_
#include "stm32f10x.h"

/*系统滴答定时器用作基本延时,延时精度1微秒*/
/*计数器为24位计数器*/

/*导入系统时钟频率*/
extern uint32_t SystemCoreClock;

void CoreTickDelayInit(void);

void CoreTickDelayMs(uint32_t nms);

void CoreTickDelayUs(uint16_t nus);




#endif





