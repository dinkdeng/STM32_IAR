#include "DeviceRS485.h"
#include "SystemUtil.h"

//485模式控制.0,接收;1,发送.
#define DeviceRS485_TxEnable(n)		    (PGout(8) = n)

DeviceRS485_CallBackFunc deviceRS485CallBackFuncPtr = NULL;

/**初始化 */
void DeviceRS485_Init(uint32_t bpsSet,DeviceRS485_CallBackFunc callBackFuncPtr)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//使能USART2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	//串口2引脚复用映射 /GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	//GPIOA3复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	//USART2   GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化PA2，PA3
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//PG8推挽输出，485模式控制  GPIOG8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	//输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化PG8
	GPIO_Init(GPIOG,&GPIO_InitStructure);

	//USART2 初始化设置 波特率设置
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
	//初始化串口2
	USART_Init(USART2, &USART_InitStructure);

	//使能串口 2
	USART_Cmd(USART2, ENABLE);

	//清除TC,否则可能第一个数据丢失
	USART_ClearFlag(USART2, USART_FLAG_TC);

	//开启接受中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=DEVICE_RS485_RX_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =DEVICE_RS485_RX_SUB_PRI;
	//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//根据指定的参数初始化VIC寄存器
	NVIC_Init(&NVIC_InitStructure);

    /**默认为接收模式 */
	DeviceRS485_TxEnable(0);
    deviceRS485CallBackFuncPtr = callBackFuncPtr;
}

/**发送字节 */
void DeviceRS485_SendChar(uint8_t byteValue)
{
	//设置为发送模式
	DeviceRS485_TxEnable(1);
    //发送数据
    USART_SendData(USART2,byteValue);
    //等待发送结束
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
	//设置为接收模式
	DeviceRS485_TxEnable(0);
}

/**发送缓冲区 */
void DeviceRS485_SendBuffer(uint8_t* bufferPtr,uint16_t length)
{
    uint16_t i = 0;
    //设置为发送模式
	DeviceRS485_TxEnable(1);
    for(i = 0; i < length; i++)
    {
        //发送数据
        USART_SendData(USART2,bufferPtr[i]);
        //等待发送结束
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
    }
    //设置为接收模式
	DeviceRS485_TxEnable(0);
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

/**发送字符串 */
uint8_t DeviceRS485_SendString(uint8_t* strPtr)
{
    int stringLength = GetStringLength(strPtr);
    if(stringLength > 0)
    {
        DeviceRS485_SendBuffer(strPtr,(uint16_t)stringLength);
        return 0;
    }
    return 1;
}

#include "CoreSerialUart1DMA.h"
/**默认的回调函数 */
void DeviceRS485_DefaultCallBackFunc(uint8_t byteValue)
{
    CoreSerialUart1DMA_SendByte(byteValue);
}

void USART2_IRQHandler(void)
{
	uint8_t res;
	//接收到数据
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//读取接收到的数据USART2->DR
		res =USART_ReceiveData(USART2);
		if(deviceRS485CallBackFuncPtr != NULL)
            deviceRS485CallBackFuncPtr(res);
	}
}