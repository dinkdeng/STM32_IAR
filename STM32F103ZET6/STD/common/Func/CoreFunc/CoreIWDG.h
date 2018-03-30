#ifndef __CORE_IWDG_H_
#define __CORE_IWDG_H_
#include "stm32f10x.h"

/**默认分频系数 40KHZ LSI 分频 4*2的四次方=64 最大256*/
#define CORE_IWDG_DEFAULT_PRER          4

/**默认重装载值 40000/64/625 = 1 默认的最大喂狗时间1S,最大2048*/
#define CORE_IWDG_DEFAULT_RELOAD        625

/**初始化 */
void CoreIWDGInit(uint8_t prer, uint16_t reloadValue);

/**定时喂狗 */
void CoreIWDGFeed(void);


#endif






