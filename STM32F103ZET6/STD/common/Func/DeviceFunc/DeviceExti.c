#include "DeviceExti.h"
#include "SystemUtil.h"

static uint16_t leftExtiCount = 0;

static uint16_t rightExtiCount = 0;

static uint16_t upExtiCount = 0;

static uint16_t downExtiCount = 0;

static void DeviceExtiLeftInit()
{
    //外部中断配置结构体
    EXTI_InitTypeDef EXTI_InitStructure;
    //中断向量配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    //按键端口初始化
    GPIO_InitTypeDef GPIO_InitStructure;

    //如果主程序没有启用这个时钟name这里就要启动复用时钟
    //使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    //PE2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
    //设置成上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIOE2
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //GPIOE.2中断线以及中断初始化配置   下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
    //中断线模式配置
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //下降沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    EXTI_Init(&EXTI_InitStructure);

    //中断通道定义 使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_LEFT_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_LEFT_SUB_PRI;
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    leftExtiCount = 0;
}

static void DeviceExtiRightInit()
{
    //外部中断配置结构体
    EXTI_InitTypeDef EXTI_InitStructure;
    //中断向量配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    //按键端口初始化
    GPIO_InitTypeDef GPIO_InitStructure;

    //如果主程序没有启用这个时钟name这里就要启动复用时钟
    //使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    //PE4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    //设置成上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIOE4
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //GPIOE.4中断线以及中断初始化配置   下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
    //中断线模式配置
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //下降沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    EXTI_Init(&EXTI_InitStructure);

    //中断通道定义 使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_RIGHT_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_RIGHT_SUB_PRI;
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    rightExtiCount = 0;
}

static void DeviceExtiUpInit()
{
    //外部中断配置结构体
    EXTI_InitTypeDef EXTI_InitStructure;
    //中断向量配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    //按键端口初始化
    GPIO_InitTypeDef GPIO_InitStructure;

    //如果主程序没有启用这个时钟name这里就要启动复用时钟
    //使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    //初始化 WK_UP-->GPIOA.0	  下拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    //PA0设置成输入，默认下拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    //初始化GPIOA.0
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //GPIOA.0 中断线以及中断初始化配置 上升沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //中断通道定义 使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_UP_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_UP_SUB_PRI;
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    upExtiCount = 0;
}

static void DeviceExtiDownInit()
{
    //外部中断配置结构体
    EXTI_InitTypeDef EXTI_InitStructure;
    //中断向量配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    //按键端口初始化
    GPIO_InitTypeDef GPIO_InitStructure;

    //如果主程序没有启用这个时钟name这里就要启动复用时钟
    //使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOE, ENABLE);

    //PE3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    //设置成上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIOE3
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //GPIOE.3中断线以及中断初始化配置   下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
    //中断线模式配置
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //下降沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    EXTI_Init(&EXTI_InitStructure);

    //中断通道定义 使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_DOWN_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_DOWN_SUB_PRI;
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    downExtiCount = 0;
}

/**外部中断初始化 */
void DeviceExtiInit(EXTI_INDEX index)
{
    switch (index)
    {
    case EXTI_LEFT:
        DeviceExtiLeftInit();
        break;
    case EXTI_RIGHT:
        DeviceExtiRightInit();
        break;
    case EXTI_UP:
        DeviceExtiUpInit();
        break;
    case EXTI_DOWN:
        DeviceExtiDownInit();
        break;
    default:
        break;
    }
}

/**获取中断触发次数 */
uint16_t DeviceExtiGetCount(EXTI_INDEX index)
{
    switch(index)
    {
    case EXTI_LEFT:
        return leftExtiCount;
    case EXTI_RIGHT:
        return rightExtiCount;
    case EXTI_UP:
        return upExtiCount;
    case EXTI_DOWN:
        return downExtiCount;
    default:
        return 0;
    }
}

/**清除中断触发次数 */
void DeviceExtiClear(EXTI_INDEX index)
{
    switch(index)
    {
    case EXTI_LEFT:
        leftExtiCount = 0;
        break;
    case EXTI_RIGHT:
        rightExtiCount = 0;
        break;
    case EXTI_UP:
        upExtiCount = 0;
        break;
    case EXTI_DOWN:
        downExtiCount = 0;
        break;
    default:
        break;
    }
}



/**中断处理函数 */
void EXTI0_IRQHandler(void)
{
    upExtiCount++;
    /*清除LINE2上的中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line0);
}

/**中断处理函数 */
void EXTI2_IRQHandler(void)
{
    leftExtiCount++;
    /*清除LINE2上的中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line2);
}

/**中断处理函数 */
void EXTI3_IRQHandler(void)
{
    downExtiCount++;
    /*清除LINE2上的中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line3);
}

/**中断处理函数 */
void EXTI4_IRQHandler(void)
{
    rightExtiCount++;
    /*清除LINE2上的中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line4);
}
