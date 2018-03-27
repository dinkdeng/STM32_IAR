#include "DeviceKey.h"
#include "CoreTickDelay.h"

//PE4
#define IO_KEY_RIGHT 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) 
//PE3
#define IO_KEY_DOWN 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	 
//PE2
#define IO_KEY_LEFT 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) 
//PA0
#define IO_KEY_UP 			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	

/*系统外部按键驱动文件*/

/*上拉 PE2*/
static void DeviceKeyLeftInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//使能GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	//普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*上拉 PE4*/
static void DeviceKeyRightInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	//使能GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	//普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*下拉 PA0*/
static void DeviceKeyUpInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	//使能GPIOA,GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	//普通输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//100M
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*上拉 PE3*/
static void DeviceKeyDownInit(void)
{
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
	//初始化GPIOE2,3,4
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void DeviceKeyInit(void)
{
    DeviceKeyLeftInit();
    DeviceKeyRightInit();
    DeviceKeyUpInit();
    DeviceKeyDownInit();
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyLeftRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyRightRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyUpRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    if (ioValue == 0)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        if (ioValue == 0)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyDownRead(void)
{
    uint8_t ioValue = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
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







