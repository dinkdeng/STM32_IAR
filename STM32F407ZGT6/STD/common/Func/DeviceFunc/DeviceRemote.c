#include "DeviceRemote.h"

//红外按键键值定义
#define REMOTE_KEY_NONE		0
#define REMOTE_KEY_POWER	162
#define REMOTE_KEY_UP		98
#define REMOTE_KEY_PLAY		2
#define REMOTE_KEY_ALIENTEK	226
#define REMOTE_KEY_RIGHT	194
#define REMOTE_KEY_LEFT		34
#define REMOTE_KEY_VOL_DEC	224
#define REMOTE_KEY_VOL_ADD	144
#define REMOTE_KEY_DOWN		168
#define REMOTE_KEY_1		104
#define REMOTE_KEY_2		152
#define REMOTE_KEY_3		176
#define REMOTE_KEY_4		48
#define REMOTE_KEY_5		24
#define REMOTE_KEY_6		122
#define REMOTE_KEY_7		16
#define REMOTE_KEY_8		56
#define REMOTE_KEY_9		90
#define REMOTE_KEY_0		66
#define REMOTE_KEY_DELETE	82

//红外按键键值对应的字符定义
const uint8_t REMOTE_KEY_ERROR_DESP[]="ERROR";
const uint8_t REMOTE_KEY_POWER_DESP[]="POWER";
const uint8_t REMOTE_KEY_UP_DESP[]="UP";
const uint8_t REMOTE_KEY_PLAY_DESP[]="PLAY"	;
const uint8_t REMOTE_KEY_ALIENTEK_DESP[]="ALIENTEK";	
const uint8_t REMOTE_KEY_RIGHT_DESP[]="RIGHT";	
const uint8_t REMOTE_KEY_LEFT_DESP[]="LEFT";
const uint8_t REMOTE_KEY_VOL_DEC_DESP[]="VOL-";
const uint8_t REMOTE_KEY_VOL_ADD_DESP[]="VOL+";
const uint8_t REMOTE_KEY_DOWN_DESP[]="DOWN";
const uint8_t REMOTE_KEY_1_DESP[]="1";
const uint8_t REMOTE_KEY_2_DESP[]="2";
const uint8_t REMOTE_KEY_3_DESP[]="3";
const uint8_t REMOTE_KEY_4_DESP[]="4";
const uint8_t REMOTE_KEY_5_DESP[]="5";
const uint8_t REMOTE_KEY_6_DESP[]="6";
const uint8_t REMOTE_KEY_7_DESP[]="7";
const uint8_t REMOTE_KEY_8_DESP[]="8";
const uint8_t REMOTE_KEY_9_DESP[]="9";
const uint8_t REMOTE_KEY_0_DESP[]="0";
const uint8_t REMOTE_KEY_DELETE_DESP[]="DELETE";	

void DeviceRemoteInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM1_ICInitStructure;

	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//TIM1时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 

	//GPIOA8  复用功能,上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIOA8复用为TIM1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); 
	//预分频器,1M的计数频率,1us加1. 168M主频,这里用167 是的一次计数1us
	TIM_TimeBaseStructure.TIM_Prescaler=167;	
	//向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	//设定计数器自动重装值 最大10ms溢出
	TIM_TimeBaseStructure.TIM_Period=10000;     
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure); 

	//初始化TIM2输入捕获参数 CC1S=01 	选择输入端 IC1映射到TI1上
	TIM1_ICInitStructure.TIM_Channel = TIM_Channel_1; 
	//上升沿捕获
	TIM1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	//映射到TI1上	
	TIM1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	//配置输入分频,不分频
	TIM1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	//IC1F=0003 8个定时器时钟周期滤波	
	TIM1_ICInitStructure.TIM_ICFilter = 0x03;
	//初始化定时器1输入捕获通道
	TIM_ICInit(TIM1, &TIM1_ICInitStructure);

	//允许更新中断 ,允许CC1IE捕获中断
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_CC1,ENABLE);	
	//使能定时器1
	TIM_Cmd(TIM1,ENABLE); 	 	

	//用作输入捕获的TIM1 
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= DEVICE_REMOTE_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_REMOTE_SUB_PRI;	
	//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化NVIC寄存器
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= DEVICE_REMOTE_PREE_PRI;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_REMOTE_SUB_PRI;
	//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化NVIC寄存器	
	NVIC_Init(&NVIC_InitStructure);	
}

uint32_t DeviceRemoteGetKey(uint8_t* keyValue)
{

}

uint8_t* DeviceRemoteGetDespWithKey(uint8_t* keyValue)
{

}





