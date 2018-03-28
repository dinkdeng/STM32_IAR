#include "DeviceKey.h"


//PE4
#define IO_KEY_RIGHT 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)
//PE3
#define IO_KEY_DOWN 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)	 
//PE2
#define IO_KEY_LEFT 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2) 
//PA0
#define IO_KEY_UP 			HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

/*系统外部按键驱动文件*/

/*上拉 PE2*/
static void DeviceKeyLeftInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟
    
	GPIO_Initure.Pin=GPIO_PIN_2;
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

/*上拉 PE4*/
static void DeviceKeyRightInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟
    
	GPIO_Initure.Pin=GPIO_PIN_4;
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

/*下拉 PA0*/
static void DeviceKeyUpInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟

    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

/*上拉 PE3*/
static void DeviceKeyDownInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟
    
	GPIO_Initure.Pin=GPIO_PIN_3;
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

/**按键初始化 */
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
    uint8_t ioValue = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyRightRead(void)
{
    uint8_t ioValue = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
        if (ioValue == 1)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyUpRead(void)
{
    uint8_t ioValue = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    if (ioValue == 0)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        if (ioValue == 0)
            return KEY_RELEASE;
        else
            return KEY_PRESS;
    }
}

/**读取按键键值 */
static KEY_STATUS DeviceKeyDownRead(void)
{
    uint8_t ioValue = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
    if (ioValue == 1)
        return KEY_RELEASE;
    else
    {
        DEVICE_KEY_DELAY_MS(DEVICE_WAIT_PRESS_STABLE_MS);
        ioValue = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
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




