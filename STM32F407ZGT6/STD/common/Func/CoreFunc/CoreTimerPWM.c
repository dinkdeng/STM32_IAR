#include "CoreTimerPWM.h"
#include "SystemUtil.h"

//TIM14 PWM部分初始化
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
//例子
//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms
//TIM3_Count_Init(5000-1,8400-1);
void CoreTimer14PWM_Init(uint32_t arr,uint16_t psc)
{
    //此部分需手动修改IO口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//TIM14时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);
	//使能PORTF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14);

	//GPIOF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化PF9
	GPIO_Init(GPIOF,&GPIO_InitStructure);

	//定时器分频
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	//向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	//自动重装载值
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	//初始化定时器14
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);

	//初始化TIM14 Channel1 PWM模式	 选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);

	//使能TIM14在CCR1上的预装载寄存器
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);

	//ARPE使能
	TIM_ARRPreloadConfig(TIM14,ENABLE);

    /*使能比较输出*/
    TIM_CCxCmd(TIM14, TIM_Channel_1, TIM_CCx_Disable);
	//使能TIM14
	TIM_Cmd(TIM14, DISABLE);
}

/**启动PWM输出 */
void CoreTimer14PWM_Start(uint32_t duty)
{
	/*设置初始化占空比*/
    TIM_SetCompare1(TIM14, duty);
	/*使能比较输出*/
    TIM_CCxCmd(TIM14, TIM_Channel_1, TIM_CCx_Enable);
    //使能TIM14
	TIM_Cmd(TIM14, ENABLE);
}

/**停止PWM输出 */
void CoreTimer14PWM_Stop(void)
{
	/*禁止比较输出*/
    TIM_CCxCmd(TIM14, TIM_Channel_1, TIM_CCx_Disable);
    //禁止TIM14
	TIM_Cmd(TIM14, DISABLE);
}

/**设置PWM输出占空比 */
void CoreTimer14PWM_SetDuty(uint32_t dutySet)
{
	uint8_t timer14OC1OpenFlag = 0;
    /*获取当前通道状态*/
    if ((TIM14->CCER)& (1 << TIM_Channel_1) != 0)
    {
        timer14OC1OpenFlag = 1;
        /*关闭比较输出*/
        TIM_CCxCmd(TIM14, TIM_Channel_1, TIM_CCx_Disable);
    }
    /*设置初始化占空比*/
    TIM_SetCompare1(TIM14, dutySet);
    /*根据之前的状态决定是否打开定时器*/
    if (timer14OC1OpenFlag == 1)
    {
        TIM_CCxCmd(TIM14, TIM_Channel_1, TIM_CCx_Enable);
    }
}



