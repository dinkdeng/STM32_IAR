#include "DeviceLed.h"

/*红色 LED初始化*/
static void DeviceLedRedInit(LED_STATUS initStatus)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    //打开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //PB5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /*执行亮灭操作*/
    (initStatus == LED_STATUS_ON)?GPIO_ResetBits(GPIOB, GPIO_Pin_5):
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

/*绿色LED初始化*/
static void DeviceLedGreenInit(LED_STATUS initStatus)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    //打开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    //PE5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    /*执行亮灭操作*/
    (initStatus == LED_STATUS_ON)?GPIO_ResetBits(GPIOE, GPIO_Pin_5):
        GPIO_SetBits(GPIOE, GPIO_Pin_5);
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
     /*执行亮灭操作*/
    (status == LED_STATUS_ON)?GPIO_ResetBits(GPIOB, GPIO_Pin_5):
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

/*设置绿色Led 亮灭*/
static void DeviceLedGreenSet(LED_STATUS status)
{
    (status == LED_STATUS_ON)?GPIO_ResetBits(GPIOE, GPIO_Pin_5):
        GPIO_SetBits(GPIOE, GPIO_Pin_5);
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
    uint8_t ioValue = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5);
    return ((ioValue == 0) ? LED_STATUS_ON : LED_STATUS_OFF);
}

/*读取绿色Led状态*/
static LED_STATUS DeviceLedGreenGet(void)
{
    uint8_t ioValue = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_5);
    return ((ioValue == 0) ? LED_STATUS_ON : LED_STATUS_OFF);
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