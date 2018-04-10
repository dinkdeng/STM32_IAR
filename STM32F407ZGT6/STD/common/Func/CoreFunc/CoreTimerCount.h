#ifndef __CORE_TIMER_COUNT_H_
#define __CORE_TIMER_COUNT_H_
#include "stm32f4xx.h"

typedef void (*CoreTimerCountIntCallBackFunc)(void);

#define CORE_TIMER3_COUNT_PREE_PRI      INT_PRE_PRI_3

#define CORE_TIMER3_COUNT_SUB_PRI       INT_SUB_PRI_3

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
//因为系统初始化 SystemInit 函数里面已经初始化 APB1 的时钟为 4 分频，所
//以 APB1 的时钟为 42M， 而从 STM32F4 的内部时钟树图（ 图 4.3.1.1）得知：当 APB1 的时钟
//分频数为 1 的时候， TIM2~7 以及 TIM12~14 的时钟为 APB1 的时钟，而如果 APB1 的时钟分频
//数不为 1，那么 TIM2~7 以及 TIM12~14 的时钟频率将为 APB1 时钟的两倍。因此， TIM3 的时
//钟为 84M
void CoreTimer3CountInit(uint16_t arr,uint16_t psc,CoreTimerCountIntCallBackFunc callBackPtr);


void CoreTimer3CountDefaultCallBack(void);


#endif





