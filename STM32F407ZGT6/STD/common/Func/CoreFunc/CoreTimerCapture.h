#ifndef __CORE_TIMER_CAPTURE_H_
#define __CORE_TIMER_CAPTURE_H_
#include "stm32f4xx.h"

#define CORE_TIMER5_CAPTURE_PREE_PRI        INT_PRE_PRI_2

#define CORE_TIMER5_CAPTURE_SUB_PRI         INT_SUB_PRI_3

/**脉冲时间抓取,默认US计时,这样比较精准,所以该函数需要根据系统CPU频率进行适配*/
void CoreTimer5CaptureInit(void);

/*获取捕获次数以及最大的捕获时间*/
uint32_t CoreTimer5CaptureGetCountWithTime(double* maxTimer);



#endif



