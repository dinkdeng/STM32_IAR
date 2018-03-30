#include "CoreWWDG.h"
#include "SystemUtil.h"

//保存WWDG计数器的设置值,默认为最大. 
u8 WWDG_CNT = 0X7F;

static void CoreWWDG_NVICInit()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    //WWDG中断
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    //抢占
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_WWDG_EWI_PREE_PRI;
    //子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_WWDG_EWI_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC初始化	
    NVIC_Init(&NVIC_InitStructure);
}

/**窗口看门狗初始化  
 * 初始化窗口看门狗 ,窗口看门狗时钟来源PCLK1 
 *  tr   :T[6:0],计数器值 
 *  wr   :W[6:0],窗口值 
 *  fprer:分频系数（WDGTB）,仅最低2位有效
 *  Fwwdg=PCLK1/(4096*2^fprer).*/
void CoreWWDGInit(uint8_t countValue, uint8_t windowValue, uint32_t fprer)
{
    //WWDG时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    //初始化WWDG_CNT.
    WWDG_CNT = countValue&WWDG_CNT;
    //设置WWDG预分频值
    WWDG_SetPrescaler(fprer);
    //设置窗口值
    WWDG_SetWindowValue(windowValue);
    //使能看门狗 ,	设置 counter
    WWDG_Enable(WWDG_CNT);
    //清除提前唤醒中断标志位 
    WWDG_ClearFlag();
    //初始化窗口看门狗 NVIC
    CoreWWDG_NVICInit();
    //开启窗口看门狗中断
    WWDG_EnableIT();
}

/**窗口看门狗喂狗 */
void CoreWWDGFeed(uint8_t countValue)
{
    //使能看门狗 ,设置 counter 
    WWDG_Enable(countValue);
}

void WWDG_IRQHandler(void)
{
    //当禁掉此句后,窗口看门狗将产生复位
    WWDG_SetCounter(WWDG_CNT);
    //清除提前唤醒中断标志位
    WWDG_ClearFlag();
}