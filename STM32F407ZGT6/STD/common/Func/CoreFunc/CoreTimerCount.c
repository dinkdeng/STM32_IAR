#include "CoreTimerCount.h"
#include "SystemUtil.h"
#include "DeviceLed.h"

CoreTimerCountIntCallBackFunc coreTimer3CountIntCallBackPtr = NULL;

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
//因为系统初始化 SystemInit 函数里面已经初始化 APB1 的时钟为 4 分频，所
//以 APB1 的时钟为 42M， 而从 STM32F4 的内部时钟树图（ 图 4.3.1.1）得知：当 APB1 的时钟
//分频数为 1 的时候， TIM2~7 以及 TIM12~14 的时钟为 APB1 的时钟，而如果 APB1 的时钟分频
//数不为 1，那么 TIM2~7 以及 TIM12~14 的时钟频率将为 APB1 时钟的两倍。因此， TIM3 的时
//钟为 84M
void CoreTimer3CountInit(uint16_t arr,uint16_t psc,CoreTimerCountIntCallBackFunc callBackPtr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能TIM3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  

	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Period = arr; 
	//定时器分频
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  
	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	//初始化TIM3
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

	//允许定时器3更新中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
	//使能定时器3
	TIM_Cmd(TIM3,ENABLE); 

	//定时器3中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=CORE_TIMER3_COUNT_PREE_PRI; 
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=CORE_TIMER3_COUNT_SUB_PRI; 

	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    coreTimer3CountIntCallBackPtr = callBackPtr;
}

void CoreTimer3CountDefaultCallBack(void)
{
    //这一句要放在最下面执行
    DeviceLedToogle(LED_INDEX_RED);
}


//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		if(coreTimer3CountIntCallBackPtr != NULL)
            coreTimer3CountIntCallBackPtr();
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}



