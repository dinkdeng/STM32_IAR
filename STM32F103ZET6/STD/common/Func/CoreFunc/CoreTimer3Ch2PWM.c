#include "CoreTimer3Ch2PWM.h"
#include "SystemUtil.h"

static uint32_t timer3CH2pwmOutCount = 0;

/**
 *  IM3 PWM部分初始化 PWM输出初始化
 *  arr：自动重装值
 *  psc：时钟预分频数
 *  PWM的频率为 72M/(arr+1)/(psc+1)
 *  80K的计数频率 TIM3_PWM_Init(899,0)
*/
void CoreTimer3Ch2PwmInit(u16 arr, u16 psc, uint16_t duty, uint32_t pwmOutCount)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    //使能定时器3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //使能GPIO外设和AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    //Timer3部分重映射  TIM3_CH2->PB5
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
    //TIM_CH2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    //复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //初始化GPIO
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;
    //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    //输出极性:TIM输出比较极性高
    //根据T指定的参数初始化外设TIM3 OC2
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    /*设置初始化占空比*/
    TIM_SetCompare2(TIM3, duty);

    //使能TIM3在CCR2上的预装载寄存器
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    /*设置想要输出的PWM数量*/
    timer3CH2pwmOutCount = pwmOutCount;

    //关闭PWM比较中断
    TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
    /**当设置为0的时候,就是持续输出*/
    if (pwmOutCount != 0)
    {
        /*打开PWM中断*/
        NVIC_InitTypeDef NVIC_InitStructure;
        //清除TIMx更新中断标志
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        //允许PWM比较中断
        TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

        //中断优先级NVIC设置 TIM3中断
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        //先占优先级
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_TIMER3_PWM_PREE_PRI;
        //从优先级3级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_TIMER3_PWM_SUB_PRI;
        //IRQ通道被使能
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        //初始化NVIC寄存器
        NVIC_Init(&NVIC_InitStructure);
    }
    /*使能比较输出*/
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);
    //使能TIM3
    TIM_Cmd(TIM3, ENABLE);
}

void CoreTimer3Ch2PwmSetDuty(uint16_t duty)
{
    uint8_t timer3OC2OpenFlag = 0;
    /*获取当前通道状态*/
    if ((TIM3->CCER)& (1 << TIM_Channel_2) != 0)
    {
        timer3OC2OpenFlag = 1;
        /*关闭比较输出*/
        TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);
    }
    /*设置初始化占空比*/
    TIM_SetCompare2(TIM3, duty);
    /*根据之前的状态决定是否打开定时器*/
    if (timer3OC2OpenFlag == 1)
    {
        TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);
    }
}

void CoreTimer3Ch2PwmStart(uint32_t pwmOutCount)
{
    /*关闭比较输出*/
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);
    //关闭PWM比较中断
    TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);

    /*设置想要输出的PWM数量*/
    timer3CH2pwmOutCount = pwmOutCount;
    if (pwmOutCount != 0)
    {
        /*打开PWM中断*/
        NVIC_InitTypeDef NVIC_InitStructure;
        //清除TIMx更新中断标志
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        //允许PWM比较中断
        TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

        //中断优先级NVIC设置 TIM3中断
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        //先占优先级
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_TIMER3_PWM_PREE_PRI;
        //从优先级3级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_TIMER3_PWM_SUB_PRI;
        //IRQ通道被使能
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        //初始化NVIC寄存器
        NVIC_Init(&NVIC_InitStructure);
    }
    /*使能比较输出*/
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);
    /*启动定时器*/
    TIM_Cmd(TIM3, ENABLE);
}

void CoreTimer3Ch2PwmStop(void)
{
    //关闭PWM比较中断
    TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
    /*关闭比较输出*/
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    //检查TIM3更新中断发生与否
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
    {
        //清除TIMx更新中断标志
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        if(timer3CH2pwmOutCount)
            timer3CH2pwmOutCount--;
        if (timer3CH2pwmOutCount == 0)
        {
            /*关闭比较输出*/
            TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);
            //关闭PWM比较中断
            TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
        }
    }
}