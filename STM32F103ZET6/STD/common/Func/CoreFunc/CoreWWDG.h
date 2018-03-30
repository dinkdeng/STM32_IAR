#ifndef __CORE_WWDG_H_
#define __CORE_WWDG_H_
#include "stm32f10x.h"

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
void CoreWWDGInit(uint8_t countValue, uint8_t windowValue, uint32_t fprer);

/**窗口看门狗喂狗 */
void CoreWWDGFeed(uint8_t countValue);


#endif





