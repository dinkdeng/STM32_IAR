#include "DevicePWM_DAC.h"

#define DEVICE_PWM_DAC_PSC      0

#define DEVICE_PWM_DAC_ARR      256

/**初始化 */
//使用TIM9_CH2输出PWM
//TIM9_CH2_PWM_Init(255,0);
//TIM4 PWM初始化, Fpwm=168M/256=656.25Khz.
void DevicePWM_DAC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//TIM9时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  
	//使能PORTA时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	

	//GPIOA3 PA3输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	//复用功能	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化PA3
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

	//GPIOA3复用位定时器9 AF3
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM9); 

	//定时器分频
	TIM_TimeBaseStructure.TIM_Prescaler=DEVICE_PWM_DAC_PSC; 
	//向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	//自动重装载值
	TIM_TimeBaseStructure.TIM_Period=DEVICE_PWM_DAC_ARR;   
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	//初始化定时器9
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);

	//初始化TIM14 Channel1 PWM模式	 
	//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	//比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	//输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_Pulse=0;
	//根据T指定的参数初始化外设TIM9 OC2
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);  

	//使能TIM9在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  

	//ARPE使能 
	TIM_ARRPreloadConfig(TIM9,ENABLE);

	//使能TIM9
	TIM_Cmd(TIM9, ENABLE); 
}

/**设置输出值,实际电压乘以1000,最大3300 */
uint8_t DevicePWM_DAC_SetValue(uint16_t value)
{
    if (value > 3300)
        return 1;
    double temp = value;
    temp /= 1000;
    temp = temp * DEVICE_PWM_DAC_ARR / 3.3;
    TIM_SetCompare2(TIM9,(uint16_t)temp);
    return 0;
}












