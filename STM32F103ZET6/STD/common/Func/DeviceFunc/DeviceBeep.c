#include "DeviceBeep.h"
#include "SystemUtil.h"

#define BEEP_IO     PBout(8)

/**蜂鸣器初始化 */
void DeviceBeepInit(BEEP_STATUS initStatus)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    //使能GPIOB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //BEEP-->PB.8 端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    //推挽输出	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //速度为50MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //根据参数初始化GPIOB.8
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    (initStatus == BEEP_OFF)?(BEEP_IO = 0):(BEEP_IO = 1);
}

/**设置蜂鸣器状态 */
void DeviceBeepSet(BEEP_STATUS setStatus)
{
    (setStatus == BEEP_OFF)?(BEEP_IO = 0):(BEEP_IO = 1);
}







