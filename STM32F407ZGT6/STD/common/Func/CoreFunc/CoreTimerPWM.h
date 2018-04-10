#ifndef __CORE_TIMER_PWM_H_
#define __CORE_TIMER_PWM_H_
#include "stm32f4xx.h"


//TIM14 PWM部分初始化
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
//例子
//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms
//TIM3_Count_Init(5000-1,8400-1);
void CoreTimer14PWM_Init(uint32_t arr,uint16_t psc);

/**启动PWM输出 */
void CoreTimer14PWM_Start(uint32_t duty);

/**停止PWM输出 */
void CoreTimer14PWM_Stop(void);

/**设置PWM输出占空比 */
void CoreTimer14PWM_SetDuty(uint32_t dutySet);


#endif





