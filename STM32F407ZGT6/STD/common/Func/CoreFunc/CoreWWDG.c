#include "CoreWWDG.h"
#include "SystemUtil.h"

//保存WWDG计数器的设置值,默认为最大.
uint8_t WWDG_CNT = 0X7F;

/**窗口看门狗初始化
 * 初始化窗口看门狗 ,窗口看门狗时钟来源PCLK1
 *  tr   :T[6:0],计数器值
 *  wr   :W[6:0],窗口值
 *  fprer:分频系数（WDGTB）,仅最低2位有效
 *  Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz.*/
void CoreWWDG_Init(uint8_t countValue, uint8_t windowValue, uint32_t fprer)
{
    NVIC_InitTypeDef NVIC_InitStructure;
	//使能窗口看门狗时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);

	//初始化WWDG_CNT.
	WWDG_CNT=countValue&WWDG_CNT;
	//设置分频值
	WWDG_SetPrescaler(fprer);
	//设置窗口值
	WWDG_SetWindowValue(windowValue);
	//	WWDG_SetCounter(WWDG_CNT);//设置计数值
	//开启看门狗
	WWDG_Enable(WWDG_CNT);

	//窗口看门狗中断
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=CORE_WWDG_EWI_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=CORE_WWDG_EWI_SUB_PRI;
	//使能窗口看门狗
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//清除提前唤醒中断标志位
	WWDG_ClearFlag();
	//开启提前唤醒中断
	WWDG_EnableIT();
}

/**窗口看门狗喂狗 */
void CoreWWDG_Feed(uint8_t countValue)
{
    //使能看门狗 ,设置 counter
    WWDG_SetCounter(countValue);
}

#include "DeviceLed.h"

void WWDG_IRQHandler(void)
{
    //重设窗口看门狗值
	WWDG_SetCounter(CORE_WWDG_DEFAULT_COUNT);
    //清除提前唤醒中断标志位
	WWDG_ClearFlag();
    //这一句要放在最下面执行
    DeviceLedToogle(LED_INDEX_RED);
}