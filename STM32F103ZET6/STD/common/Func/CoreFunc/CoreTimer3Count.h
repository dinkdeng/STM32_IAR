#ifndef __CORE_TIMER3_COUNT_H_
#define __CORE_TIMER3_COUNT_H_
#include "stm32f10x.h"

#define CORE_TIMER3_COUNT_PREE_PRI      INT_PRE_PRI_3

#define CORE_TIMER3_COUNT_SUB_PRI       INT_SUB_PRI_3

/*中断回调指针*/
typedef void(*CoreTimer3CountCallBackPtr)(void);

/*初始化*/
void CoreTimer3CountInit(uint16_t arr, uint16_t psc, CoreTimer3CountCallBackPtr callBackFunc);

/*启动计数*/
void CoreTimer3CountStart(void);

/*停止计数*/
void CoreTimer3CountStop(void);




#endif




