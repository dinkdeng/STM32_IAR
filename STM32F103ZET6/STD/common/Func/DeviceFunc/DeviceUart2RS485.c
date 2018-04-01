#include "DeviceUart2RS485.h"
#include "SystemUtil.h"

//模式控制
//485模式控制.0,接收;1,发送.
#define DEVICE_UART2_RS485_TX_EN		PGout(9)	

/**485接口初始化 */
void DeviceUart2RS485Init(uint32_t bps)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //使能GPIOA,G时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOG, ENABLE);
    //使能USART2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //PG9端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
    //推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    //PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    //复用推挽
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    //浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //复位串口2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);
    //停止复位
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE);

    //一般设置为9600
    USART_InitStructure.USART_BaudRate = bps;
    //8位数据长度
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    //一个停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    //奇偶校验位
    USART_InitStructure.USART_Parity = USART_Parity_No;
    //无硬件数据流控制
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    //收发模式
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    //初始化串口
    USART_Init(USART2, &USART_InitStructure);
    //使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 
    //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_UART2_RS485_PREE_PRI; 
    //从优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_UART2_RS485_SUB_PRI; 
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    NVIC_Init(&NVIC_InitStructure); 

    //开启中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    //使能串口 
    USART_Cmd(USART2, ENABLE); 
    //清除TC,避免第一个数据发送不出去
    USART_GetFlagStatus(USART2, USART_FLAG_TC);
    //默认为接收模式
    DEVICE_UART2_RS485_TX_EN = 0;		
}

/**发送一个字节 */
void DeviceUart2RS485SendChar(uint8_t chValue)
{
    /*设置为发送模式*/
    DEVICE_UART2_RS485_TX_EN = 1;
    USART_SendData(USART2, chValue);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    //设置为接收模式	
    DEVICE_UART2_RS485_TX_EN = 0;
}

/**发送一个缓冲区 */
void DeviceUart2RS485SendBuffer(uint8_t* bufferPtr,uint16_t bufferLength)
{
    uint16_t t;
    /*设置为发送模式*/
    DEVICE_UART2_RS485_TX_EN = 1;	
    //循环发送数据
    for (t = 0; t < bufferLength; t++)
    {
        USART_SendData(USART2, bufferPtr[t]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    }
    //设置为接收模式	
    DEVICE_UART2_RS485_TX_EN = 0;	
}

static int StrGetLength(uint8_t* strPtr)
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

/**发送一个字符串 */
void DeviceUart2RS485SendString(uint8_t* strPtr)
{
    int stringLength = StrGetLength(strPtr);
    if(stringLength > 0)
    {
        DeviceUart2RS485SendBuffer(strPtr,(uint16_t)stringLength);
    }
}

static uint16_t recvDataLength = 0;
static uint8_t deviceUart2RS485RecvBuffer[DEVICE_UART2_RS485_BUFFER_MAX] = {0};

/**获取接收到的缓存 */
uint16_t DeviceUart2RS485GetRecv(uint8_t** recvBufferPtr)
{
    *recvBufferPtr = deviceUart2RS485RecvBuffer;
    return recvDataLength;
}

/**清除接收缓冲 */
void DeviceUart2RS485ClearRecv(void)
{
    recvDataLength = 0;
}

void USART2_IRQHandler(void)
{
    uint8_t res;
    //接收到数据
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
    {
        //读取接收到的数据
        res = USART_ReceiveData(USART2); 	
        if(recvDataLength < DEVICE_UART2_RS485_BUFFER_MAX)
        {
            deviceUart2RS485RecvBuffer[recvDataLength] = res;
        }
        else
        {
            /**超过最大长度,不再接收 */
            return;
        }
    }
}





