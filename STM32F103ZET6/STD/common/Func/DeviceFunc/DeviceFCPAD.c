#include "DeviceFCPAD.h"
#include "SystemUtil.h"

/**时钟讯号设置*/
#define DeviceFCPAD_ClkSet(n)           (PCout(12) = n)

/**锁存信号设置*/
#define DeviceFCPAD_LatSet(n)           (PCout(8) = n)

/**数据读取*/
#define DeviceFCPAD_DatRead()           PCin(9)


/**设备初始化*/
void DeviceFCPAD_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    //使能PC端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 
    //PC8 12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_12;	
    //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //初始化GPIO
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    //上拉
    GPIO_SetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_12);	

    //PC9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    //上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    //初始化GPIO
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    //上拉
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
}

uint8_t DeviceFCPAD_ReadKey(void)
{
    uint8_t temp = 0;
    uint8_t t;
    //锁存当前状态
    DeviceFCPAD_LatSet(1);					
    DeviceFCPAD_LatSet(0);
    for (t = 0; t < 8; t++)
    {
        temp <<= 1;
        //LOAD之后，就得到第一个数据
        if (DeviceFCPAD_DatRead())
            temp |= 0x01;
        //每给一次脉冲，收到一个数据
        DeviceFCPAD_ClkSet(1);			   	
        DeviceFCPAD_ClkSet(0);
    }
    return temp;
}



