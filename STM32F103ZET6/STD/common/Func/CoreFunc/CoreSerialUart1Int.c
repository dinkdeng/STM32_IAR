#include "CoreSerialUart1Int.h"
#include "SystemUtil.h"

/**初始化 */
void CoreSerialUart1IntInit(uint32_t bpsSet)
{
    //gpio初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    //串口初始化结构体
    USART_InitTypeDef USART_InitStructure;
    //中断向量配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;

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

    //中断通道定义 使能串口的中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_INT_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_INT_SUB_PRI;
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //初始化USART
    USART_Init(USART1, &USART_InitStructure);
    //接收中断使能
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //使能对应中断
    NVIC_EnableIRQ(USART1_IRQn);
    //使能串口模块
    USART_Cmd(USART1, ENABLE);
    //清除TC,避免第一个数据发送不出去
    USART_GetFlagStatus(USART1, USART_FLAG_TC);
}

/**发送字节码 */
void CoreSerialUart1IntSendChar(uint8_t chValue)
{
    /* 等待数据发送完成 ，传输完成标志置位*/
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* 向相应串口发送数据*/
    USART_SendData(USART1, chValue);
}

/**发送缓冲区 */
void CoreSerialUart1IntSendBuffer(uint8_t* bufferPtr,uint16_t bufferLength)
{
    uint16_t lengthLocal = bufferLength;
    while (lengthLocal)
    {
        CoreSerialUart1IntSendChar(bufferPtr[bufferLength - lengthLocal]);
        lengthLocal--;
    }
}

/**获取字符串长度 */
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

/**发送字符串 */
void CoreSerialUart1IntSendString(uint8_t* strPtr)
{
    int stringLength = GetStrinLength(strPtr);
    if(stringLength > 0)
    {
        CoreSerialUart1IntSendBuffer(strPtr,(uint16_t)stringLength);
    }
}

/**串口接收函数需要使用的内部数据 */

/**数据缓冲区 */
static uint8_t uart1RecvBuffer[UART1_RX_MAX_BUFFER_SIZE] = {0};

/**数据计数区 */
/**0-15位存放数据计数 */
/**16位存放接收到回车 */
/**17位存放接收到换行 */
/**接收到换行代表一帧数据帧接受完成 */
static uint32_t uart1RecvState = 0;

static void Uart1RecvCallBack(uint8_t recvCh)
{
    /**已经接收到回车 */
    if(uart1RecvState&(1<<16))
    {
        /**接收到回车,下一个必须是换行 */
        if(recvCh == '\n')
        {
            /**标识一帧数据接收完成 */
            uart1RecvState |= (1<<17);
            /**关闭中断,等待后续处理 */
            NVIC_DisableIRQ(USART1_IRQn);
        }
        else
        {
            /**数据格式错误,前面接收到的状态全面清除 */
            uart1RecvState = 0;
            return;
        }
    }
    else
    {
        /**还没接收到回车 */
        if(recvCh == '\r')
        {
            /**接收到回车,设置状态值 */
            uart1RecvState |= (1<<16);
        }
        else
        {
            /**数据长度还未超过长度限制 */
            if((uart1RecvState & 0x0000FFFF) < UART1_RX_MAX_BUFFER_SIZE)
            {
                uart1RecvBuffer[(uart1RecvState & 0x0000FFFF)] = recvCh;
                uart1RecvState++;
            }
            else
            {
                /**数据格式超出最大长度限制,这一帧被清除掉 */
                uart1RecvState = 0;
                return;
            }
        }
    }
}

/**清除接受缓存 */
void CoreSerialUart1IntClearRx(void)
{
    uart1RecvState = 0;
    /**打开中断 */
    NVIC_EnableIRQ(USART1_IRQn);
}

/**获取当前接收的一包数据 */
uint16_t CoreSerialUart1IntGetRecv(uint8_t** recvBuffer)
{
    if((uart1RecvState &(1ul<<17)))
    {
        *recvBuffer = uart1RecvBuffer;
        return (uint16_t)(uart1RecvState & 0x0000FFFF);
    }
    else
    {
        *recvBuffer = NULL;
        return 0;
    }
}

/**串口中断函数 */
void USART1_IRQHandler(void)
{
    uint8_t Res;
    //接收中断(接收到的数据必须是0x0d 0x0a \r\n回车换行结尾)
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        //读取接收到的数据
        Res = USART_ReceiveData(USART1);
        Uart1RecvCallBack(Res);
    }
    else
    {
        return;
    }
}
