#include "CoreIWDG.h"


//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).
void CoreIWDG_Init(uint8_t prer, uint16_t reloadValue)
{
    //使能对寄存器IWDG_PR和IWDG_RLR的写操作
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //设置IWDG预分频值:设置IWDG预分频值为64
    IWDG_SetPrescaler(prer);
    //设置IWDG重装载值
    IWDG_SetReload(reloadValue);
    //按照IWDG重装载寄存器的值重装载IWDG计数器
    IWDG_ReloadCounter();
    //使能IWDG
    IWDG_Enable();
}

/**定时喂狗 */
void CoreIWDG_Feed(void)
{
    //reload
    IWDG_ReloadCounter();
}


