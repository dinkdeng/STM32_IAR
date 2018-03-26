#include "CoreSerial1.h"
#include "SystemUtil.h"

/*232串口驱动程序*/

/**串口初始化,带接收中断*/
void CoreSerial1Init(uint32_t bpsSet)
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_SUB_PRI;
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

/**串口发送一个字节 */
void CoreSerial1SendByte(uint8_t dat)
{
    /* 等待数据发送完成 ，传输完成标志置位*/
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* 向相应串口发送数据*/
    USART_SendData(USART1, dat);
}

/**串口发送一个数组 */
void CoreSerial1SendBuffer(uint8_t* datPtr,uint16_t length)
{
    u16 lengthLocal = length;
    while (lengthLocal)
    {
        CoreSerial1SendByte(datPtr[length - lengthLocal]);
        lengthLocal--;
    }
}

/**串口发送一个字符串 */
void CoreSerial1SendString(uint8_t* strPtr)
{
    uint32_t stringLength = 0;
    for(stringLength = 0; stringLength < 60000; stringLength++)
    {
        if(*(strPtr+stringLength) == '\0')
            break;
    }
    CoreSerial1SendBuffer(strPtr,stringLength);
}


/*串口的缓冲区*/
uint8_t coreSerial1RXBuffer[64] = { 0 };

/** 保存当前串口接收数据状态,
 * 0-13位 数据长度
 * 14位 是否已经接收到回车
 * 15位 是否已经接收到换行. */
uint16_t coreSerial1RxStatus = 0;


/**串口中断接收处理函数 */
void CoreSerial1IntProcess()
{
    u8 Res;
    //接收中断(接收到的数据必须是0x0d 0x0a \r\n回车换行结尾)
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        //(USART1->DR);	//读取接收到的数据
        Res = USART_ReceiveData(USART1);
        /*已经接收到回车,等待接收换行*/
        if (CORE_UART1_CHECK_RX_STATUS_CR())
        {
            /*接收到换行,设置接收到换行标识符*/
            if (Res == '\n')
            {
                CORE_UART1_SET_RX_STATUS_LF();
            }
            else
            {
                /*清除当前串口接收状态,从0开始接收,相当于重新初始化*/
                CORE_UART1_CLEAR_STATUS();
            }
        }
        else if (CORE_UART1_CHECK_RX_STATUS_LF())
        {
            /*接收到换行,等待主程序处理,此时接收到任何数据都不处理*/
            return;
        }
        /*既没接收到回车也没接收到换行,而且此刻数据总量已经超过了最大接收量*/
        else if (64 == (coreSerial1RxStatus & 0x00FF))
        {
            /*清除接受状态,重新开始接收,至于回车丢了这种特殊情况,我们可以依靠协议来解决,
            而不需要在中断层做过于复杂的处理,降低中断延迟*/
            CORE_UART1_CLEAR_STATUS();
        }
        else
        {
            /*既没接收到回车也没接收到换行,此时正常接收到回车,表示回车标志*/
            if (Res == '\r')
            {
                CORE_UART1_SET_RX_STATUS_CR();
            }
            else
            {
                /*不是回车,那么正常接收*/
                coreSerial1RXBuffer[(coreSerial1RxStatus & 0x00FF)] = Res;
                /*接收之后将接收到的字符串的总数量+1*/
                coreSerial1RxStatus++;
            }
        }
    }
    else
    {
        return;
    }
}

/*中断向量*/
void USART1_IRQHandler(void)
{
    CoreSerial1IntProcess();
}

