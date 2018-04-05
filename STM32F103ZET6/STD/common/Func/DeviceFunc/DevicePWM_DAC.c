#include "DevicePWM_DAC.h"

#define DEVICE_PWM_DAC_PSC      0

#define DEVICE_PWM_DAC_ARR      256

/**初始化 */
void DevicePWM_DAC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    //使能TIMx外设
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //使能GPIOB外设时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


    //设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    //复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //初始化GPIO
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //设置自动重装载周期值
    TIM_TimeBaseStructure.TIM_Period = DEVICE_PWM_DAC_ARR;
    //设置预分频值 不分频
    TIM_TimeBaseStructure.TIM_Prescaler = DEVICE_PWM_DAC_PSC;
    //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //根据指定的参数初始化TIMx
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    //CH1 PWM2模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    //比较输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_Pulse = 1;
    //OC1 低电平有效
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    //根据指定的参数初始化外设TIMx
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);

    //CH1 预装载使能
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    //使能TIMx在ARR上的预装载寄存器
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    //使能TIMx
    TIM_Cmd(TIM4, ENABLE);
}

/**设置输出值,实际电压乘以1000,最大3300 */
uint8_t DevicePWM_DAC_SetValue(uint16_t value)
{
    if (value > 3300)
        return 1;
    double temp = value;
    temp /= 1000;
    temp = temp * DEVICE_PWM_DAC_ARR / 3.3;
    TIM_SetCompare1(TIM4, (uint16_t)temp);
    return 0;
}












