#ifndef __CORE_TIMER3_CH2_PWM_H_
#define __CORE_TIMER3_CH2_PWM_H_
#include "stm32f10x.h"

#define CORE_TIMER3_PWM_PREE_PRI      INT_PRE_PRI_3

#define CORE_TIMER3_PWM_SUB_PRI       INT_SUB_PRI_3

void CoreTimer3Ch2PwmInit(u16 arr, u16 psc, uint16_t duty, uint32_t pwmOutCount);

void CoreTimer3Ch2PwmSetDuty(uint16_t duty);

void CoreTimer3Ch2PwmStart(uint32_t pwmOutCount);

void CoreTimer3Ch2PwmStop(void);


#endif



