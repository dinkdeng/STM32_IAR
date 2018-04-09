#ifndef __CORE_IWDG_H_
#define __CORE_IWDG_H_
#include "stm32f4xx.h"

/**默认分频系数 32KHZ LSI 分频 4*2的四次方=64 最大256 32K/64 = 500*/
#define CORE_IWDG_DEFAULT_PRER          4

/**默认重装载值 32000/64/500 = 1 默认的最大喂狗时间1S,最大2048*/
#define CORE_IWDG_DEFAULT_RELOAD        500

/**初始化 */
//IWDG初始化
//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).
//预分频数为64,重载值为500,溢出时间为1s
//IWDG_Init(4,500); 
void CoreIWDG_Init(uint8_t prer, uint16_t reloadValue);

/**定时喂狗 */
void CoreIWDG_Feed(void);


#endif






