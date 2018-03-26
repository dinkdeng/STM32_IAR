#include "CoreTickDelay.h"

/*us延时倍乘数*/
static uint32_t fac_us = 0;

/*延时初始化*/
void CoreTickDelayInit(void)
{
    //清空控制寄存器
    CLEAR_REG(SysTick->CTRL);
    //SysTick频率为HCLK
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    uint32_t hclkFreq = HAL_RCC_GetHCLKFreq();
    //不论是否使用OS,fac_us都需要使用
    fac_us = (hclkFreq / 1000000);
}

/*延时毫秒*/
/*为了不重复使用DelayUS来进行计时,充分提高延时函数精度*/
/*最大化利用systick计时,所以专门写一个以硬件级别的毫秒延时函数*/
/*延时nms 注意nms的范围*/
/*SysTick->LOAD为24位寄存器,所以,最大延时为:*/
/*nms<=0xffffff**1000/SystemCoreClock*/
/*SYSCLK单位为Hz,nms单位为ms*/
/*对216M条件下,nms<=233ms */
void CoreTickDelayXMs(uint32_t nms)
{
    uint32_t temp = 0;
    /*时间加载*/
    WRITE_REG(SysTick->LOAD, nms*fac_us * 1000U);
    /*清空计数器*/
    WRITE_REG(SysTick->VAL, 0U);
    /*开始倒数*/
    MODIFY_REG(SysTick->CTRL, (SysTick_CTRL_ENABLE_Msk << SysTick_CTRL_ENABLE_Pos), 1);
    /*等待时间到达*/
    do
    {
        temp = READ_REG(SysTick->CTRL);
    } while ((temp & 0x01) && !(temp&SysTick_CTRL_COUNTFLAG_Msk));
    /*关闭计数器*/
    MODIFY_REG(SysTick->CTRL, (SysTick_CTRL_ENABLE_Msk << SysTick_CTRL_ENABLE_Pos), 0);
    /*清空计数器*/
    WRITE_REG(SysTick->VAL, 0);
}

/*延时毫秒*/
void CoreTickDelayMs(uint32_t nms)
{
    /*这里用200,没有用到极限233,防止超频使用*/
    uint8_t repeat = nms / 200;
    uint16_t remain = nms % 200;
    /*尽量降低函数的调用频率*/
    while (repeat)
    {
        CoreTickDelayXMs(200);
        repeat--;
    }
    if (remain)
        CoreTickDelayXMs(remain);
}

/**72M时钟,计数值72,24位
 * 计数器,最大值0XFFFFFF,.233016US最大值 */
void CoreTickDelayFUs(uint16_t nus)
{
    uint32_t temp = 0;
    /*时间加载*/
    WRITE_REG(SysTick->LOAD, nus*fac_us);
    /*清空计数器*/
    WRITE_REG(SysTick->VAL, 0);
    /*开始倒数*/
    MODIFY_REG(SysTick->CTRL, (SysTick_CTRL_ENABLE_Msk << SysTick_CTRL_ENABLE_Pos), 1);
    /*等待时间到达*/
    do
    {
        temp = READ_REG(SysTick->CTRL);
    } while ((temp & 0x01) && !(temp&SysTick_CTRL_COUNTFLAG_Msk));
    /*关闭计数器*/
    MODIFY_REG(SysTick->CTRL, (SysTick_CTRL_ENABLE_Msk << SysTick_CTRL_ENABLE_Pos), 0);
    /*清空计数器*/
    WRITE_REG(SysTick->VAL, 0);
}


