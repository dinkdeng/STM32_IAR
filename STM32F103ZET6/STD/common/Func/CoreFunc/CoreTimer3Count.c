#include "CoreTimer3Count.h"
#include "SystemUtil.h"

static CoreTimer3CountCallBackPtr callBackPtr = NULL;

/*初始化
 * 通用定时器3中断初始化 这里时钟选择为APB1的2倍，而APB1为36M
 *  arr：自动重装值。
 *  psc：时钟预分频数 这里使用的是定时器3! 例子
 *  CoreTimer3CountInit(4999,7199);
 *  72M主频分频7200 主频 10K 也就是说100US 计数值4999+1 = 5000, 中断周期500MS.*/
void CoreTimer3CountInit(uint16_t arr, uint16_t psc, CoreTimer3CountCallBackPtr callBackFunc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /*先禁用定时器*/
    TIM_Cmd(TIM3, DISABLE);
    //时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    //定时器TIM3初始化
    //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Period = arr;
    //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //根据指定的参数初始化TIMx的时间基数单位
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //清除TIMx更新中断标志
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    //中断优先级NVIC设置 TIM3中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_TIMER3_COUNT_PREE_PRI;
    //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_TIMER3_COUNT_SUB_PRI;
    //IRQ通道被使能
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //初始化NVIC寄存器
    NVIC_Init(&NVIC_InitStructure);
    /*建立函数指针连接*/
    callBackPtr = callBackFunc;

    //使能指定的TIM3中断,允许更新中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    /*初始化不打开定时器*/
    TIM_Cmd(TIM3, ENABLE);
}

/*启动计数*/
void CoreTimer3CountStart(void)
{
    /*启动定时器*/
    TIM_Cmd(TIM3, ENABLE);
}

/*停止计数*/
void CoreTimer3CountStop(void)
{
    /*停止定时器*/
    TIM_Cmd(TIM3, DISABLE);
}

void TIM3_IRQHandler(void)
{
    //检查TIM3更新中断发生与否
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        //清除TIMx更新中断标志
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        if (callBackPtr == NULL)
            return;
        callBackPtr();
    }
    
}