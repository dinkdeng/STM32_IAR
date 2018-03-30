#include "DeviceKey.h"
#include "CoreTickDelay.h"
#include "SystemUtil.h"

/*KEY LEFT PE2*/
/*KEY DOWN PE3*/
/*KEY RIGHT PE4*/
/*KEY UP PA0*/

/**外部按键端口位段定义 */
#define DEVICE_KEY_LEFT     PEin(2)
#define DEVICE_KEY_RIGHT    PEin(4)
#define DEVICE_KEY_UP       PAin(0)
#define DEVICE_KEY_DOWN     peIN(3)


/*系统外部按键驱动文件*/

/*上拉*/
static void DeviceKeyLeftInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    //PE4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    //设置成上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIOE4
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*上拉*/
static void DeviceKeyRightInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    //PE4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    //设置成上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIOE4
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*下拉*/
static void DeviceKeyUpInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    //初始化 WK_UP-->GPIOA.0	  下拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    //PA0设置成输入，默认下拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    //初始化GPIOA.0
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*上拉*/
static void DeviceKeyDownInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    //PE4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    //设置成上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIOE4
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void DeviceKeyInit(void)
{
    DeviceKeyLeftInit();
    DeviceKeyRightInit();
    DeviceKeyUpInit();
    DeviceKeyDownInit();
}

static KEY_STATUS DeviceKeyLeftRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DeviceWaitKeyStable();
        ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

static KEY_STATUS DeviceKeyRightRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DeviceWaitKeyStable();
        ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

static KEY_STATUS DeviceKeyUpRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    if (ioValue == 0)
        return KEY_RELEASE;
    else
    {
        DeviceWaitKeyStable();
        ioValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        if (ioValue == 0)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

static KEY_STATUS DeviceKeyDownRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DeviceWaitKeyStable();
        ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

KEY_STATUS DeviceKeyRead(KEY_INDEX index)
{
    switch (index)
    {
    case KEY_LEFT:
        return DeviceKeyLeftRead();
    case KEY_RIGHT:
        return DeviceKeyRightRead();
    case KEY_UP:
        return DeviceKeyUpRead();
    case KEY_DOWN:
        return DeviceKeyDownRead();
    }
    return KEY_RELEASE;
}
