#ifndef __CORE_WWDG_H_
#define __CORE_WWDG_H_
#include "stm32f4xx.h"

/**提前预中断优先级 */
#define CORE_WWDG_EWI_PREE_PRI      INT_PRE_PRI_0

#define CORE_WWDG_EWI_SUB_PRI       INT_SUB_PRI_0

/**默认装载值 */
#define CORE_WWDG_DEFAULT_COUNT     0X7F

/**默认窗口值 */
#define CORE_WWDG_WINDOW_VALUE      0X5F

/**默认分频值 Fwwdg=PCLK1/(4096*2^fprer)*/
#define CORE_WWDG_PRER              WWDG_Prescaler_8

/**窗口看门狗初始化 */
//初始化窗口看门狗 	
//countValue   :T[6:0],计数器值 
//windowValue   :W[6:0],窗口值 
//fprer:分频系数（WDGTB）,仅最低2位有效 
//Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
void CoreWWDG_Init(uint8_t countValue, uint8_t windowValue, uint32_t fprer);

/**窗口看门狗喂狗 */
void CoreWWDG_Feed(uint8_t countValue);


#endif





