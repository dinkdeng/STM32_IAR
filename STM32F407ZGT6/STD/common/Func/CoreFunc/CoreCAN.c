#include "CoreCAN.h"
#include "SystemUtil.h"

/**内置的中断处理函数*/
static CoreCAN_CallbackFunc rxCallBackFunc = NULL;

//CAN总线初始化参数列表
//参数分别是 重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//计算公式为  波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//Fpclk1的时钟在初始化的时候设置为42M
static const uint8_t coreCAN_BpsTqArray[3][4] = {{CAN_SJW_1tq,CAN_BS1_7tq,CAN_BS2_6tq,6}, \
								                 {CAN_SJW_1tq,CAN_BS1_7tq,CAN_BS2_6tq,12}, \
								                 {CAN_SJW_1tq,CAN_BS1_7tq,CAN_BS2_6tq,24}};


/**初始化 */
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//Fpclk1的时钟在初始化的时候设置为42M,CoreCAN_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;
void CoreCAN_Init(CORE_CAN_BPS bps,CoreCAN_CallbackFunc callbackFunc)
{
	//IO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	//CAN初始化配置
	CAN_InitTypeDef        CAN_InitStructure;
	//过滤器配置
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	//中断配置
	NVIC_InitTypeDef  NVIC_InitStructure;

	//使能相关时钟 使能PORTA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//使能CAN1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化PA11,PA12
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//引脚复用映射配置 GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1);
	//GPIOA12复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1);

	//CAN单元设置 非时间触发通信模式
	CAN_InitStructure.CAN_TTCM=DISABLE;
	//软件自动离线管理
	CAN_InitStructure.CAN_ABOM=DISABLE;
	//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_AWUM=DISABLE;
	//禁止报文自动传送
	CAN_InitStructure.CAN_NART=ENABLE;
	//报文不锁定,新的覆盖旧的
	CAN_InitStructure.CAN_RFLM=DISABLE;
	//优先级由报文标识符决定
	CAN_InitStructure.CAN_TXFP=DISABLE;
	//模式设置 正常收发
	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;
    //CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;
	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_SJW=coreCAN_BpsTqArray[bps][0];
	//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS1=coreCAN_BpsTqArray[bps][1];
	//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_BS2=coreCAN_BpsTqArray[bps][2];
	//分频系数(Fdiv)为brp+1
	CAN_InitStructure.CAN_Prescaler=coreCAN_BpsTqArray[bps][3];
	// 初始化CAN1
	CAN_Init(CAN1, &CAN_InitStructure);

	//配置过滤器 过滤器0 过滤master flag
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	//指定掩码过滤
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	//32位
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	//32位ID 高16位
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	//低16位
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	//32位MASK 高16位
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	//低16位
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	//激活过滤器0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	//过滤器初始化
	CAN_FilterInit(&CAN_FilterInitStructure);
	//FIFO0消息挂号中断允许.
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);

	//can接收中断,初始化的时候不开接收中断,直到系统正常初始化完成才开接收中断
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	// 主优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CORE_CAN_RX_PREE_PRI;
	// 次优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CORE_CAN_RX_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    rxCallBackFunc = callbackFunc;
}

/**发送数据 */
void CoreCAN_SendMessage(CORE_CAN_MSG_TYPE type,uint32_t canID,uint8_t* data,uint8_t len)
{
    CanTxMsg TxMessage;
    if(type == CORE_CAN_MSG_STD)
    {
        TxMessage.IDE = CAN_ID_STD;
        TxMessage.StdId = canID;
    }
    else
    {
        TxMessage.IDE = CAN_ID_EXT;
        TxMessage.ExtId=canID;
    }
    // 消息类型为数据帧，一帧8位
	TxMessage.RTR=CAN_RTR_Data;
	// 发送两帧信息
	TxMessage.DLC=len;
    for(uint8_t i=0;i<len;i++)
	{
		TxMessage.Data[i]=data[i];
	}
    /*发送这一帧看是否发送成功*/
	CAN_Transmit(CAN1, &TxMessage);
    /**等待传输结束*/
    while((CAN_TransmitStatus(CAN1, 0)==CAN_TxStatus_Failed)&&(CAN_TransmitStatus(CAN1, 1)==CAN_TxStatus_Failed)&& \
					(CAN_TransmitStatus(CAN1, 2)==CAN_TxStatus_Failed));
}


//中断服务函数
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg rxMsg;
    /**做一个标识,后续检测这个看看是否读到了数据*/
    rxMsg.DLC = 255;
    /**默认用邮箱0*/
    CAN_Receive(CAN1, 0, &rxMsg);
    /*清除挂号中断*/
    CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    if(rxMsg.DLC != 255 && rxCallBackFunc != NULL)
    {
        rxCallBackFunc(&rxMsg);
    }
}

#include <stdio.h>

void ShowCanRecvPrintf(CanRxMsg* messagePtr)
{
    printf("rxMsg.IDE:  %08x\r\n", messagePtr->IDE);
    printf("rxMsg.RTR  %08x\r\n", messagePtr->RTR);
    printf("rxMsg.StdId  %08x\r\n", messagePtr->StdId);
    printf("rxMsg.ExtId  %08x\r\n", messagePtr->ExtId);
    printf("rxMsg.DLC  %08x\r\n", messagePtr->DLC);
    printf("rxMsg.Data : \r\n");
    for (uint8_t i = 0; i < messagePtr->DLC; i++)
    {
        printf("  %02x  ", messagePtr->Data[i]);
    }
    printf("\r\n");
}