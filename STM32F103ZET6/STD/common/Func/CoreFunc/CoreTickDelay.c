#include "CoreTickDelay.h"

/*系统滴答定时器用作基本延时,延时精度1微秒*/

/*us延时倍乘数,tick延时1US,tick定时器计数的个数*/
static u8  facUs = 0;
/*ms延时倍乘数,tick延时1MS,Tick定时器计数的个数*/
static u16 facMs = 0;


void CoreTickDelayInit(void)
{
    /*系统时钟更新*/
    SystemCoreClockUpdate();
    /*设置SYSTICK定时器*/
    /*HCLK的八分频 HCLK == sysClk*/
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    /*1us的计数值 */
    facUs = (SystemCoreClock / 8) / 1000000;
    /*ms计数值 us的一千倍*/
    facMs = (u16)facUs * 1000;
}

/*因为MS延时,可能造成定时器溢出,所以需要这么一个原子处理*/
/*该函数只能内部调用,调用的最大值就可以控制,根据实际情况进行修改,就不会造成溢出*/
static void CoreTickDelayXMs(uint32_t nms)
{
    u32 temp;
    //时间加载(SysTick->LOAD为24bit)
    SysTick->LOAD = (u32)nms*facMs;
    //清空计数器
    SysTick->VAL = 0x00;
    //开始倒数
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    //等待时间到达
    do
    {
        temp = SysTick->CTRL;
    } 
    while ((temp & 0x01) && !(temp&(1 << 16)));
    //关闭计数器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    //清空计数器
    SysTick->VAL = 0X00;
}

void CoreTickDelayMs(uint32_t nms)
{
    //这里用1200,没有用到极限1864,防止超频使用
    u8 repeat = nms / 1200;
    u16 remain = nms % 1200;
    //尽量降低函数的调用频率
    while (repeat)
    {
        CoreTickDelayXMs(1200);
        repeat--;
    }
    if (remain)
        CoreTickDelayXMs(remain);
}




/*US延时,Tick定时器24位,最大支持1864135,65535完全支持*/
void CoreTickDelayUs(uint16_t nus)
{
    u32 temp;
    //时间加载
    SysTick->LOAD = nus * facUs;
    //清空计数器
    SysTick->VAL = 0x00;
    //开始倒数
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    //等待时间到达
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp&(1 << 16)));
    //关闭计数器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    //清空计数器
    SysTick->VAL = 0X00;
}