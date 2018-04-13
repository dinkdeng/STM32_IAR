#include "DeviceTouchKey.h"
#include "CoreTickDelay.h"


//最大的ARR值(TIM2是32位定时器)
#define TPAD_ARR_MAX_VAL  0XFFFFFFFF


//空载的时候(没有手按下),计数器需要的时间
//这个值应该在每次开机的时候被初始化一次
volatile uint32_t tpadDefaultVal;


//定时器2通道2输入捕获配置
//arr：自动重装值
//psc：时钟预分频数
void DeviceTouchKeyTimer2CH1_CapInit(u32 arr,u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM2_ICInitStructure;

	//TIM2时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	//使能PORTA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//GPIOA5复用位定时器2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_TIM2);

	//GPIOA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//不带上下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//初始化PA5
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//初始化TIM2 设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Period = arr;
	//预分频器
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//TIM向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	//初始化通道1 CC1S=01 	选择输入端 IC1映射到TIM2上
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1;
	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	//配置输入分频,不分频
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	//IC2F=0000 配置输入滤波器 不滤波
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;
	//初始化TIM2 IC1
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//使能定时器2
	TIM_Cmd(TIM2,ENABLE);
}


//复位一次
//释放电容电量，并清除定时器的计数值
void DeviceTouchKeyReset(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	//普通输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	//初始化PA5
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//输出0,放电
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);

	CoreTickDelayMs(5);
	//清除中断标志
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update);
	//计数值归0
	TIM_SetCounter(TIM2,0);

	//PA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	//复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//推挽
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//不带上下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//初始化PA5
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

uint16_t DeviceTouchKeyGetVal(void)
{
	DeviceTouchKeyReset();
	//等待捕获上升沿
	while(TIM_GetFlagStatus(TIM2, TIM_IT_CC1) == RESET)
	{
		//超时了,直接返回CNT的值
		if(TIM_GetCounter(TIM2)>TPAD_ARR_MAX_VAL-500)
			return TIM_GetCounter(TIM2);
	}
	return TIM_GetCapture1(TIM2);
}

uint16_t DeviceTouchKeyGetMaxVal(u8 n)
{
	uint16_t temp=0;
	uint16_t res=0;
	while(n--)
	{
		//得到一次值
		temp=DeviceTouchKeyGetVal();
		if(temp>res)
			res=temp;
	};
	return res;
}

/**按键初始化 */
//初始化触摸按键
//获得空载的时候触摸按键的取值.
//systick:分频系数,越小,灵敏度越高.
//返回值:0,初始化成功;1,初始化失败
uint8_t DeviceTouchKeyInit(uint8_t sensitivity)
{
    uint16_t buf[10];
	uint16_t temp;
	uint8_t j,i;
	//设置分频系数
	DeviceTouchKeyTimer2CH1_CapInit(TPAD_ARR_MAX_VAL,sensitivity-1);
	for(i=0;i<10;i++)//连续读取10次
	{
		buf[i]=DeviceTouchKeyGetVal();
		CoreTickDelayMs(10);
	}
	for(i=0;i<9;i++)//排序
	{
		//冒泡排序
		for(j=i+1;j<10;j++)
		{
			//升序排列
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	//取中间的8个数据进行平均
	for(i=2;i<8;i++)
		temp+=buf[i];
	tpadDefaultVal=temp/6;
	//初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常!
	if(tpadDefaultVal>TPAD_ARR_MAX_VAL/2)
		return 1;
	return 0;
}

//触摸的门限值,也就是必须tpadDefaultVal+TPAD_GATE_VAL,才认为是有效触摸.
//单位是计数值
#define TPAD_GATE_VAL 	100

/**按键扫描 */
uint8_t DeviceTouchKeyScan(DEVICE_TOUCH_KEY_MODE mode)
{
    //0,可以开始检测;>0,还不能开始检测
	static uint8_t keyen=0;
	uint8_t res=0;
	//默认采样次数为3次
	uint8_t sample=3;
	uint8_t rval;
	if(mode == TOUCH_KEY_MODE_CONTINUE)
	{
		//支持连按的时候，设置采样次数为6次
		sample=6;
		//支持连按
		keyen=0;
	}
	rval=DeviceTouchKeyGetMaxVal(sample);
	//大于tpad_default_val+TPAD_GATE_VAL,且小于10倍tpad_default_val,则有效
	if(rval>(tpadDefaultVal+TPAD_GATE_VAL)&&rval<(10*tpadDefaultVal))
	{
		//大于tpad_default_val+TPAD_GATE_VAL,有效
		if((keyen==0)&&(rval>(tpadDefaultVal+TPAD_GATE_VAL)))
		{
			res=1;
		}
		//至少要再过3次之后才能按键有效
		keyen=3;
	}
	if(keyen)
		keyen--;
	return res;
}


