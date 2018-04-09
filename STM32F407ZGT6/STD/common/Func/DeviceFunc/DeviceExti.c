#include "DeviceExti.h"
#include "SystemUtil.h"

//PE4
#define DeviceExtiRight() 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
//PE3
#define DeviceExtiDown() 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
//PE2
#define DeviceExtiLeft() 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
//PA0
#define DeviceExtiUp() 			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

static uint16_t leftExtiCount = 0;

static uint16_t rightExtiCount = 0;

static uint16_t upExtiCount = 0;

static uint16_t downExtiCount = 0;

/**PE2 */
static void DeviceExtiLeftInit()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	//使能GPIOA,GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	//普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化GPIOE
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//使能SYSCFG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//PE2 连接到中断线2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);

	/* 配置EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	//中断事件
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//下降沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//中断线使能
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//配置
	EXTI_Init(&EXTI_InitStructure);

	/***************中断向量的初始化***********************/
	//外部中断2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_LEFT_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_LEFT_SUB_PRI;
	//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//配置
	NVIC_Init(&NVIC_InitStructure);

    leftExtiCount = 0;
}

/**PE4 */
static void DeviceExtiRightInit()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	//使能GPIOA,GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	//普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化GPIOE
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//使能SYSCFG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//PE4 连接到中断线4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);

	/* 配置EXTI_Line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	//中断事件
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//下降沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//中断线使能
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//配置
	EXTI_Init(&EXTI_InitStructure);

	/***************中断向量的初始化***********************/
	//外部中断2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_RIGHT_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_RIGHT_SUB_PRI;
	//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//配置
	NVIC_Init(&NVIC_InitStructure);

    rightExtiCount = 0;
}

/**PA0 */
static void DeviceExtiUpInit()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	//使能GPIOA,GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//WK_UP对应引脚PA0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    //普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//下拉,高电平触发
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	//初始化GPIOA0
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//使能SYSCFG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//PA0 连接到中断线0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/*************外部中断外设的初始化*********************/
	/* 配置EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	//中断事件
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//上升沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	//使能LINE0
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//配置
	EXTI_Init(&EXTI_InitStructure);

	/***************中断向量的初始化***********************/
	//外部中断0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_UP_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_UP_SUB_PRI;
	//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//配置
	NVIC_Init(&NVIC_InitStructure);

    upExtiCount = 0;
}

/**PE3 */
static void DeviceExtiDownInit()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	//使能GPIOA,GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	//普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化GPIOE
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//使能SYSCFG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//PE4 连接到中断线4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);

	/* 配置EXTI_Line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	//中断事件
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//下降沿触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//中断线使能
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//配置
	EXTI_Init(&EXTI_InitStructure);

	/***************中断向量的初始化***********************/
	//外部中断2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_DOWN_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_DOWN_SUB_PRI;
	//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//配置
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




//外部中断0服务程序 up
void EXTI0_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET)
	{
		upExtiCount++;
	}
	//清除LINE0上的中断标志位
	EXTI_ClearITPendingBit(EXTI_Line0);
}

//外部中断4服务程序 right
void EXTI4_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==Bit_RESET)
	{
		rightExtiCount++;
	}
	//清除LINE4上的中断标志位
	EXTI_ClearITPendingBit(EXTI_Line4);
}

//外部中断3服务程序 down
void EXTI3_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==Bit_RESET)
	{
		downExtiCount++;
	}
	//清除LINE3上的中断标志位
	EXTI_ClearITPendingBit(EXTI_Line3);
}

//外部中断2服务程序 left
void EXTI2_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) ==Bit_RESET)
	{
		leftExtiCount++;
	}
	//清除LINE2上的中断标志位
	EXTI_ClearITPendingBit(EXTI_Line2);
}

