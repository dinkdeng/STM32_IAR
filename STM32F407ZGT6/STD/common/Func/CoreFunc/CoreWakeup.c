#include "CoreWakeup.h"
#include "SystemUtil.h"

/**初始化 */
void CoreWakeupInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /**使能GPIOA时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
    /**使能SYSCFG时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    //PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);         //初始化

    /**PA0 连接到中断线0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); 

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             //LINE0
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //使能LINE0
    EXTI_Init(&EXTI_InitStructure);                        //配置

    /**外部中断0 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;             
    /**抢占优先级 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_WAKEUP_PREE_PRI;
    /**子优先级*/ 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_WAKEUP_SUB_PRI;  
    /**使能外部中断通道*/      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              
    /**配置NVIC */
    NVIC_Init(&NVIC_InitStructure);                              
}

/**进入待机 */
void CoreWakeUpEnterStandBy(void)
{
    /**等待WK_UP按键松开(在有RTC中断时,必须等WK_UP松开再进入待机) */
    while (1 == PAin(0));

    RCC_AHB1PeriphResetCmd(0X04FF, ENABLE); //复位所有IO口

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //使能PWR时钟

    PWR_BackupAccessCmd(ENABLE); //后备区域访问使能

    //这里我们就直接关闭相关RTC中断
    RTC_ITConfig(RTC_IT_TS | RTC_IT_WUT | RTC_IT_ALRB | RTC_IT_ALRA, DISABLE); //关闭RTC相关中断，可能在RTC实验打开了。
    RTC_ClearITPendingBit(RTC_IT_TS | RTC_IT_WUT | RTC_IT_ALRB | RTC_IT_ALRA); //清楚RTC相关中断标志位。

    PWR_ClearFlag(PWR_FLAG_WU); //清除Wake-up 标志

    PWR_WakeUpPinCmd(ENABLE); //设置WKUP用于唤醒

    PWR_EnterSTANDBYMode(); //进入待机模式
}

//中断,检测到PA0脚的一个上升沿.	  
//中断线0线上的中断检测
void EXTI0_IRQHandler(void)
{
    /**清除LINE10上的中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line0); 
} 