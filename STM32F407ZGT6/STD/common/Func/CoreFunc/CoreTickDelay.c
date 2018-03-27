#include "CoreTickDelay.h"

//us延时倍乘数
static uint16_t  fac_us=0;
//ms延时倍乘数,应当是fac_us的1000倍
static uint16_t fac_ms=0;

/**系统延时函数初始化 */
void CoreTickDelayInit(void)
{
    //主频八分频
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	//不论是否使用OS,fac_us都需要使用
	fac_us=(u8)(SystemCoreClock/(8000000));
	//非OS下,代表每个ms需要的systick时钟数
	fac_ms=(u16)fac_us*1000;
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对168M条件下,nms<=798ms
void CoreTickDelayXMs(uint32_t nms)
{
    u32 temp;
	//时间加载(SysTick->LOAD为24bit)
	SysTick->LOAD=(u32)nms*fac_ms;
	//清空计数器
	SysTick->VAL =0x00;
	//开始倒数
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	//等待时间到达
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	//关闭计数器
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	//清空计数器
	SysTick->VAL =0X00;
}

/**系统延时毫秒 */
void CoreTickDelayMs(uint32_t nms)
{
    //这里用540,没有用到极限798,防止超频使用
	u8 repeat=nms/540;

	//比如超频到248M的时候,CoreTickDelayXMs最大只能延时541ms左右了
	u16 remain=nms%540;
	//尽量降低函数的调用频率
	while(repeat)
	{
		CoreTickDelayXMs(540);
		repeat--;
	}
	if(remain)
		CoreTickDelayXMs(remain);
}

/**系统延时微秒 */
/** 延时nus
//nus为要延时的us数.
//注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21) */
void CoreTickDelayUs(uint16_t nus)
{
    u32 temp;
	//时间加载
	SysTick->LOAD=nus*fac_us;
	//清空计数器
	SysTick->VAL=0x00;
	//开始倒数
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	//等待时间到达
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	//关闭计数器
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	//清空计数器
	SysTick->VAL =0X00;
}

