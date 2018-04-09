#include "CoreSerialUart1Int.h"
#include "SystemUtil.h"

CoreSerialUart1IntCallBackFunc coreSerialUart1CallBackFunc = NULL;

/**初始化函数*/
void CoreSerialUart1IntInit(uint32_t bpsSet,CoreSerialUart1IntCallBackFunc callBackFuncPtr)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//使能USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	//串口1对应引脚复用映射 GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	//GPIOA10复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	//USART1端口配置 GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
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

	//开启相关中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//Usart1 NVIC 配置 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=CORE_SERIAL_UART1_PERR_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =CORE_SERIAL_UART1_SUB_PRI;
	//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//根据指定的参数初始化VIC寄存器
	NVIC_Init(&NVIC_InitStructure);
    coreSerialUart1CallBackFunc = callBackFuncPtr;
}

/**发送字节*/
void CoreSerialUart1IntSendByte(uint8_t byteValue)
{
    //循环发送,直到发送完毕
	while((USART1->SR&0X40)==0);
	USART1->DR = (uint8_t) byteValue;
}

/**发送缓冲区*/
void CoreSerialUart1IntSendBuffer(uint8_t* bufferPtr,uint16_t length)
{
    uint16_t lengthLocal = length;
	while(lengthLocal)
	{
		CoreSerialUart1IntSendByte(bufferPtr[length-lengthLocal]);
		lengthLocal--;
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
uint8_t CoreSerialUart1IntSendString(uint8_t* strPtr)
{
    int stringLength = GetStringLength(strPtr);
    if(stringLength > 0)
    {
        CoreSerialUart1IntSendBuffer(strPtr,(uint16_t)stringLength);
        return 0;
    }
    return 1;
}

void CoreSerialUart1IntDefaultCallBack(uint8_t byteValue)
{
    CoreSerialUart1IntSendByte(byteValue);
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
		if(coreSerialUart1CallBackFunc == NULL)
            return;
        else
            CoreSerialUart1IntDefaultCallBack(res);
	}
}


/*重定向系统printf库.*/
int putchar(int ch)
{
    CoreSerialUart1IntSendByte(ch);
    return ch;
}