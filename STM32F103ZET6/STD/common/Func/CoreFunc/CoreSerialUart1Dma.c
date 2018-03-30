#include "CoreSerialUart1Dma.h"
#include "SystemUtil.h"

/*DMA结构体*/
static DMA_InitTypeDef DMA_InitStructure;

uint8_t dmaTransFirstFlag = 1;

/**初始化 */
void CoreSerialUart1DmaInit(uint32_t bpsSet)
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
    /*使能DMA传输*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /*将DMA的通道1寄存器重设为缺省值*/
    DMA_DeInit(DMA1_Channel4);

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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_DMA_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_DMA_SUB_PRI;
    //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*DMA外设基地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
    /*DMA内存基地址*/
    DMA_InitStructure.DMA_MemoryBaseAddr = NULL;
    /*数据传输方向，从内存读取发送到外设*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    /*DMA通道的DMA缓存的大小*/
    DMA_InitStructure.DMA_BufferSize = 0;
    /*外设地址寄存器不变*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    /*内存地址寄存器递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /*数据宽度为8位*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    /*数据宽度为8位*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    /*工作在正常模式*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /*DMA通道 x拥有中优先级 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    /*DMA通道x没有设置为内存到内存传输*/
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    //初始化USART
    USART_Init(USART1, &USART_InitStructure);
    //接收中断使能
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //使能对应中断
    NVIC_EnableIRQ(USART1_IRQn);
    /*根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器*/
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    /*关闭USART1 TX DMA1 所指示的通道 */
    DMA_Cmd(DMA1_Channel4, DISABLE);
    //使能串口模块
    USART_Cmd(USART1, ENABLE);
     /*使能串口1的DMA发送*/
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    /*清除通道4传输完成标志*/
    DMA_ClearFlag(DMA1_FLAG_TC4);
    //清除TC,避免第一个数据发送不出去
    USART_GetFlagStatus(USART1, USART_FLAG_TC);
    /**设置DMA第一次传输标志 */
    dmaTransFirstFlag = 1;
}

/**发送字节码 */
void CoreSerialUart1DmaSendChar(uint8_t chValue)
{
    /* 等待数据发送完成 ，传输完成标志置位*/
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* 向相应串口发送数据*/
    USART_SendData(USART1, chValue);
}

/**发送缓冲区 */
void CoreSerialUart1DmaSendBuffer(uint8_t* bufferPtr,uint16_t bufferLength)
{
    if (dmaTransFirstFlag == 0)
    {
        /*等待通道4传输完成*/
        while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
        /*清除通道4传输完成标志*/
        DMA_ClearFlag(DMA1_FLAG_TC4);
    }
    dmaTransFirstFlag = 0;
    /*关闭USART1 TX DMA1 所指示的通道*/
    DMA_Cmd(DMA1_Channel4, DISABLE);
    /*DMA内存基地址*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)bufferPtr;
    /*DMA通道的DMA缓存的大小*/
    DMA_InitStructure.DMA_BufferSize = bufferLength;
    /*根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器*/
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    /*使能USART1 TX DMA1 所指示的通道 */
    DMA_Cmd(DMA1_Channel4, ENABLE);
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
void CoreSerialUart1DmaSendString(uint8_t* strPtr)
{
    int stringLength = GetStrinLength(strPtr);
    if(stringLength > 0)
    {
        CoreSerialUart1DmaSendBuffer(strPtr,(uint16_t)stringLength);
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
void CoreSerialUart1DmaClearRx(void)
{
    uart1RecvState = 0;
    /**打开中断 */
    NVIC_EnableIRQ(USART1_IRQn);
}

/**获取当前接收的一包数据 */
uint16_t CoreSerialUart1DmaGetRecv(uint8_t** recvBuffer)
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



