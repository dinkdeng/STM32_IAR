#include "DeviceLed.h"

#define LED_RED     PFout(9)
#define LED_GREEN   PFout(10)	

/*红色 LED初始化*/
static void DeviceLedRedInit(LED_STATUS initStatus)
{
    //初始化IO口需要的结构体
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//GPIOF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//普通输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化GPIO
	GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    (initStatus == LED_STATUS_ON)?GPIO_ResetBits(GPIOF,GPIO_Pin_9):GPIO_SetBits(GPIOF,GPIO_Pin_9);
}

/*绿色LED初始化*/
static void DeviceLedGreenInit(LED_STATUS initStatus)
{
    //初始化IO口需要的结构体
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//GPIOF10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//普通输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化GPIO
	GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    (initStatus == LED_STATUS_ON)?GPIO_ResetBits(GPIOF,GPIO_Pin_10):GPIO_SetBits(GPIOF,GPIO_Pin_10);
}

/*系统LED初始化*/
void DeviceLedInit(LED_STATUS initStatus)
{
    DeviceLedRedInit(initStatus);
    DeviceLedGreenInit(initStatus);
}

/*设置红色LED设置*/
static void DeviceLedRedSet(LED_STATUS status)
{
    (status == LED_STATUS_ON)?GPIO_ResetBits(GPIOF,GPIO_Pin_9):GPIO_SetBits(GPIOF,GPIO_Pin_9);
}

/*设置绿色Led 亮灭*/
static void DeviceLedGreenSet(LED_STATUS status)
{
    (status == LED_STATUS_ON)?GPIO_ResetBits(GPIOF,GPIO_Pin_10):GPIO_SetBits(GPIOF,GPIO_Pin_10);
}

/*设置LED状态*/
void DeviceLedSet(LED_INDEX index, LED_STATUS status)
{
    switch (index)
    {
    case LED_INDEX_RED:
        DeviceLedRedSet(status);
        break;
    case LED_INDEX_GREEN:
        DeviceLedGreenSet(status);
        break;
    }
}

/*读取红色Led状态*/
static LED_STATUS DeviceLedRedGet(void)
{
    return (Bit_SET==GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_9))?LED_STATUS_OFF:LED_STATUS_ON;
}

/*读取绿色Led状态*/
static LED_STATUS DeviceLedGreenGet(void)
{
     return (Bit_SET==GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_10))?LED_STATUS_OFF:LED_STATUS_ON;
}

/*获取当前LED状态*/
LED_STATUS DeviceLedGet(LED_INDEX index)
{
    switch (index)
    {
    case LED_INDEX_RED:
        return DeviceLedRedGet();
    case LED_INDEX_GREEN:
        return DeviceLedGreenGet();
    }
    return LED_STATUS_OFF;
}


/*指定LED状态切换*/
void DeviceLedToogle(LED_INDEX index)
{
    LED_STATUS status = DeviceLedGet(index);
    DeviceLedSet(index, (status == LED_STATUS_OFF ? LED_STATUS_ON : LED_STATUS_OFF));
}