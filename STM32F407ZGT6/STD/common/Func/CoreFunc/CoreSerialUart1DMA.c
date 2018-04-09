#include "CoreSerialUart1DMA.h"
#include "SystemUtil.h"

CoreSerialUart1DMA_CallBackFunc coreSerialUart1DMA_callBackFuncPtr = NULL;

uint8_t dmaTransFirst = 0;

/**初始化*/
void CoreSerialUart1DMA_Init(uint32_t bpsSet,CoreSerialUart1DMA_CallBackFunc callBackFuncPtr)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//DMA发送数据 使用DMA2_STREAM7
    /**DMA传输需要的结构体 */
	DMA_InitTypeDef  DMA_InitStructure;

	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//使能USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//DMA2时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

	//串口1对应引脚复用映射 GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	//GPIOA10复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	//USART1端口配置 GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化PA9，PA10
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//USART1 初始化设置 波特率设置
	USART_InitStructure.USART_BaudRate = bpsSet;
	//字长为8位数据格式
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//一个停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//无奇偶校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//无硬件数据流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//收发模式
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//初始化串口1
	USART_Init(USART1, &USART_InitStructure);
	//使能串口1
	USART_Cmd(USART1, ENABLE);
	//将第一个发送完成中断清除,防止第一个数据出现乱码
	USART_ClearFlag(USART1, USART_FLAG_TC);

	//DMA初始化
	DMA_DeInit(DMA2_Stream7);
	//等待DMA可配置
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
	/* 配置 DMA Stream */
	//通道选择
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	//DMA外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
	//DMA 存储器0地址 初始化设置为0 运行阶段动态调整
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)0;
	//存储器到外设模式
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	//数据传输量 初始化设置为0 运行时动态调整
	DMA_InitStructure.DMA_BufferSize = 0;
	//外设非增量模式
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//存储器增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据长度:8位
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//存储器数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// 使用普通模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//中等优先级
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	//是否开启DMA FIFO模式 不开启
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	//DMA FIFO阈值 可以选择四分之一 二分之一或者1 因为前面没开启DMA所以不关系
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	//存储器突发单次传输
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	//初始化DMA Stream
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	//使能串口1的DMA发送
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);

	//开启相关中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//Usart1 NVIC 配置 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=CORE_SERIAL_UART1_DMA_PERR_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =CORE_SERIAL_UART1_DMA_SUB_PRI;
	//IRQ通道禁止 暂时性的先关闭中断,等到系统初始化完成,再打开中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	//根据指定的参数初始化VIC寄存器
	NVIC_Init(&NVIC_InitStructure);

    /**初始化状态*/
    coreSerialUart1DMA_callBackFuncPtr = callBackFuncPtr;
    dmaTransFirst = 0;
}

/**发送字节*/
void CoreSerialUart1DMA_SendByte(uint8_t byteValue)
{
    //循环发送,直到发送完毕
	while((USART1->SR&0X40)==0);
	USART1->DR = (uint8_t) byteValue;
}

/**发送缓冲区*/
void CoreSerialUart1DMA_SendBuffer(uint8_t* bufferPtr,uint16_t length)
{
    if(dmaTransFirst == 0)
    {
        //关闭DMA传输
        DMA_Cmd(DMA2_Stream7, DISABLE);
        //确保DMA可以被设置
        while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
        //重设内存地址
        DMA_MemoryTargetConfig(DMA2_Stream7,(uint32_t)bufferPtr,DMA_Memory_0);
        //数据传输量
        DMA_SetCurrDataCounter(DMA2_Stream7,length);
        //开启DMA传输
        DMA_Cmd(DMA2_Stream7, ENABLE);
        dmaTransFirst = 1;
    }
    else
    {
        //等待上一次DMA传输完成
        while(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)==RESET);
        //清除DMA2_Steam7传输完成标志
        DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
        //关闭DMA传输
        DMA_Cmd(DMA2_Stream7, DISABLE);
        //确保DMA可以被设置
        while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
        //重设内存地址
        DMA_MemoryTargetConfig(DMA2_Stream7,(uint32_t)bufferPtr,DMA_Memory_0);
        //数据传输量
        DMA_SetCurrDataCounter(DMA2_Stream7,length);
        //开启DMA传输
        DMA_Cmd(DMA2_Stream7, ENABLE);
    }
}

static int GetStringLength(uint8_t* strPtr)
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

/**发送字符串*/
uint8_t CoreSerialUart1DMA_SendString(uint8_t* strPtr)
{
    int stringLength = GetStringLength(strPtr);
    if(stringLength > 0)
    {
        CoreSerialUart1DMA_SendBuffer(strPtr,(uint16_t)stringLength);
        return 0;
    }
    return 1;
}

void CoreSerialUart1DMA_DefaultCallBack(uint8_t byteValue)
{
    CoreSerialUart1DMA_SendByte(byteValue);
}

//串口1中断服务程序
void USART1_IRQHandler(void)
{
	u8 res;
	//接收中断(接收到的数据必须是0x0d 0x0a \r\n回车换行结尾)
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//(USART1->DR);	//读取接收到的数据
		res =USART_ReceiveData(USART1);
		if(coreSerialUart1DMA_callBackFuncPtr == NULL)
            return;
        else
            coreSerialUart1DMA_callBackFuncPtr(res);
	}
}

/**系统的printf支持,如果使用该实现,需要选择Full库模式,同时选中semihosted功能*/
/**同时为printf加上多字节支持,否则请使用单字节的putchar,不要使用DMA*/
size_t __write(int handle,const unsigned char *buf,size_t bufSize)
{
    if(bufSize == 0)
        return 0;
    if(bufSize == 1)
    {
        CoreSerialUart1DMA_SendByte(*buf);
        return 1;
    }
    else
    {
        CoreSerialUart1DMA_SendBuffer((uint8_t*)buf,(uint16_t)bufSize);
        return bufSize;
    }
    /**出错返回-1*/
}