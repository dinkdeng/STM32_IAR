#include "DeviceBeep.h"

/**蜂鸣器初始化 */
void DeviceBeepInit(BEEP_STATUS initStatus)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟

	//初始化蜂鸣器对应引脚GPIOF8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	//普通输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	//初始化GPIO
	GPIO_Init(GPIOF, &GPIO_InitStructure);

    (BEEP_OFF == initStatus)?GPIO_ResetBits(GPIOF,GPIO_Pin_8):
        GPIO_SetBits(GPIOF,GPIO_Pin_8);

}

/**蜂鸣器设置 */
void DeviceBeepSet(BEEP_STATUS set)
{
    (BEEP_OFF == set)?GPIO_ResetBits(GPIOF,GPIO_Pin_8):
        GPIO_SetBits(GPIOF,GPIO_Pin_8);
}







