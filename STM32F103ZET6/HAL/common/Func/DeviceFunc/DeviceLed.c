#include "DeviceLed.h"


/*红色 LED初始化*/
static void DeviceLedRedInit(LED_STATUS initStatus)
{
    /*PB5为红色*/
    GPIO_InitTypeDef  GPIO_InitStructRedLed;
    /*使能GPIOB端口*/
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /*设置GPIO属性*/
    GPIO_InitStructRedLed.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructRedLed.Pull = GPIO_PULLUP;
    GPIO_InitStructRedLed.Pin = GPIO_PIN_5;
    GPIO_InitStructRedLed.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructRedLed);
    /*设置初始化电平*/
    if (initStatus == LED_STATUS_ON)
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}

/*绿色LED初始化*/
static void DeviceLedGreenInit(LED_STATUS initStatus)
{
    /*PE5为绿色*/
    static GPIO_InitTypeDef  GPIO_InitStructGreenLed;
    /*使能GPIOB端口*/
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /*设置GPIO属性*/
    GPIO_InitStructGreenLed.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructGreenLed.Pull = GPIO_PULLUP;
    GPIO_InitStructGreenLed.Pin = GPIO_PIN_5;
    GPIO_InitStructGreenLed.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructGreenLed);
    /*设置初始化电平*/
    if (initStatus == LED_STATUS_ON)
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
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
     if (status == LED_STATUS_ON)
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}

/*设置绿色Led 亮灭*/
static void DeviceLedGreenSet(LED_STATUS status)
{
    if (status == LED_STATUS_ON)
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
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
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
    if (pinState == GPIO_PIN_RESET)
        return LED_STATUS_ON;
    else
        return LED_STATUS_OFF;
}

/*读取绿色Led状态*/
static LED_STATUS DeviceLedGreenGet(void)
{
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
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