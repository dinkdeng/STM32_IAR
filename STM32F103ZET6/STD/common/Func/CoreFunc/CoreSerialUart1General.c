#include "CoreSerialUart1General.h"


/**串口初始化,不打开中断 */
void CoreSerialUart1GeneralInit(uint32_t bpsSet)
{
    //gpio初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    //串口初始化结构体
    USART_InitTypeDef USART_InitStructure;

    //使能串口所有GPIO模块时钟,uart模块时钟,并使能AFIO模块时钟 ,uart1位于apb2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO |
        RCC_APB2Periph_USART1, ENABLE);

    //下面这些都是一样的配置，所以放在判断之外
    //波特率
    USART_InitStructure.USART_BaudRate = bpsSet;
    //8位数据位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    //1位停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    //奇偶校验失能
    USART_InitStructure.USART_Parity = USART_Parity_No;
    //硬件流控失能
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    //发送与接收都使能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //io模块配置pa9 设置TX引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    //复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //pa10 设置RX引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    //io配置完成
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //初始化USART
    USART_Init(USART1, &USART_InitStructure);
    //接收中断使能
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //使能串口模块
    USART_Cmd(USART1, ENABLE);
    //清除TC,避免第一个数据发送不出去
    USART_GetFlagStatus(USART1, USART_FLAG_TC);
}

/**串口发送一个字节 */
void CoreSerialUart1GeneralSendChar(uint8_t chValue)
{
    /* 等待数据发送完成 ，传输完成标志置位*/
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* 向相应串口发送数据*/
    USART_SendData(USART1, chValue);
}

/**串口发送一个缓冲区 */
void CoreSerialUart1GeneralSendBuffer(uint8_t* bufferPtr,uint16_t bufferLength)
{
    uint16_t lengthLocal = bufferLength;
    while (lengthLocal)
    {
        CoreSerialUart1GeneralSendChar(bufferPtr[bufferLength - lengthLocal]);
        lengthLocal--;
    }
}

static int GetStrinLength(uint8_t* strPtr)
{
    int stringLength = 0;
    while(stringLength < 60000)
    {
        if('\0' == *strPtr)
            break;
        else
        {
            strPtr++;
            stringLength++;
        }
    }
    if(stringLength == 60000)
        return -1;
    else
        return stringLength;
}

/**串口发送字符串 */
void CoreSerialUart1GeneralSendString(uint8_t* strPtr)
{
    int stringLength = GetStrinLength(strPtr);
    if(stringLength > 0)
    {
        CoreSerialUart1GeneralSendBuffer(strPtr,(uint16_t)stringLength);
    }
}














