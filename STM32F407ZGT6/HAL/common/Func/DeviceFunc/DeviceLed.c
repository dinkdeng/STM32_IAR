#include "DeviceLed.h"


/*红色 LED初始化*/
static void DeviceLedRedInit(LED_STATUS initStatus)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOF_CLK_ENABLE();           //开启GPIOF时钟
	
    GPIO_Initure.Pin=GPIO_PIN_9;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    (initStatus == LED_STATUS_ON)?HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET):HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);
}

/*绿色LED初始化*/
static void DeviceLedGreenInit(LED_STATUS initStatus)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOF_CLK_ENABLE();           //开启GPIOF时钟
	
    GPIO_Initure.Pin=GPIO_PIN_10;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    (initStatus == LED_STATUS_ON)?HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET):HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
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
    (status == LED_STATUS_ON)?HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET):HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);
}

/*设置绿色Led 亮灭*/
static void DeviceLedGreenSet(LED_STATUS status)
{
    (status == LED_STATUS_ON)?HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET):HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
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
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);
    if (pinState == GPIO_PIN_RESET)
        return LED_STATUS_ON;
    else
        return LED_STATUS_OFF;
}

/*读取绿色Led状态*/
static LED_STATUS DeviceLedGreenGet(void)
{
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_10);
    if (pinState == GPIO_PIN_RESET)
        return LED_STATUS_ON;
    else
        return LED_STATUS_OFF;
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

