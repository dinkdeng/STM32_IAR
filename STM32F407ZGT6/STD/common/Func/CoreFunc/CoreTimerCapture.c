#include "CoreTimerCapture.h"
#include "SystemUtil.h"

TIM_ICInitTypeDef  TIM5_ICInitStructure;

/**缓存捕获次数和时间 */
static uint32_t devicePluseCaptureCount = 0;

/**缓存捕获次数和时间 ,单位是微秒*/
static double devicePluseCaptureTime = 0;

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
// 输入捕获状态
uint8_t  devicePluseCaptureState = 0;

/*输入捕获值(TIM5是32位)*/
uint32_t devicePluseCaptureVal;

/**脉冲时间抓取,默认US计时,这样比较精准,所以该函数需要根据系统CPU频率进行适配*/
/**TIMER5是32位的定时器 */
void CoreTimer5CaptureInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//TIM5时钟使能  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 
	//使能PORTA时钟		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	

	//GPIOA0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	//复用功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//速度100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	//推挽复用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	//下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
	//初始化PA0
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

	//PA0复用位定时器5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); 

	//定时器分频,做到1us定时器增减一次
	TIM_TimeBaseStructure.TIM_Prescaler=83;  
	//向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	//自动重装载值
	TIM_TimeBaseStructure.TIM_Period=0xFFFFFFFF;  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);

	//初始化TIM5输入捕获参数 CC1S=01 	选择输入端 IC1映射到TI1上
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; 
	//上升沿捕获
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	//映射到TI1上	
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	//配置输入分频,不分频 
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	//IC1F=0000 配置输入滤波器 不滤波	
	TIM5_ICInitStructure.TIM_ICFilter = 0x0e;
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);

	//允许更新中断 ,允许CC1IE捕获中断	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);

    TIM_SetCounter(TIM5,0);
	//使能定时器5
	TIM_Cmd(TIM5,ENABLE ); 	

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=CORE_TIMER5_CAPTURE_PREE_PRI;
	//子优先级	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =CORE_TIMER5_CAPTURE_SUB_PRI;
	//IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	//根据指定的参数初始化NVIC寄存器	
	NVIC_Init(&NVIC_InitStructure);	
}



/*获取捕获次数以及最大的捕获时间*/
uint32_t CoreTimer5CaptureGetCountWithTime(double* maxTimer)
{
    uint32_t count = 0;
    *maxTimer = devicePluseCaptureTime;
    count = devicePluseCaptureCount;
    devicePluseCaptureTime = 0;
    devicePluseCaptureCount = 0;
    return count;
}

/**抓取到一次脉冲之后用于自动重启抓取 */
static void RestartCapture(void)
{
    //关闭定时器5
    TIM_Cmd(TIM5,DISABLE); 	
    TIM_SetCounter(TIM5,0);

    devicePluseCaptureCount++;
    double temp = 0;
    temp = (double)((double)(devicePluseCaptureVal) + ((devicePluseCaptureState & 0X3F) * (double)(0xFFFFFFFF)));
    if (temp > devicePluseCaptureTime)
    {
        devicePluseCaptureTime = temp;
    }
    /**初始化状态 */
    devicePluseCaptureState = 0;
    devicePluseCaptureVal = 0; 

    //使能定时器5				
    TIM_Cmd(TIM5,ENABLE ); 	
}

//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 		    
	//还未成功捕获	
	if((devicePluseCaptureState&0X80)==0)
	{
		//溢出
		if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
		{	   
			//已经捕获到高电平了
			if(devicePluseCaptureState&0X40)
			{
				//高电平太长了
				if((devicePluseCaptureState&0X3F)==0X3F)
				{
					//标记成功捕获了一次
					devicePluseCaptureState|=0X80;		
					devicePluseCaptureVal=0XFFFFFFFF;

                    /**成功捕获之后需要进入下一步捕获 */
                    //CC1P=0 设置为上升沿捕获
				    TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);
                    /**重新开始下一步捕获 */
                    RestartCapture();
				}
				else 
					devicePluseCaptureState++;
			}	 
		}
		//捕获1发生捕获事件
		if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
		{	
			//捕获到一个下降沿 	
			if(devicePluseCaptureState&0X40)			
			{	  	
				//标记成功捕获到一次高电平脉宽
				devicePluseCaptureState|=0X80;		
				//获取当前的捕获值.
				devicePluseCaptureVal=TIM_GetCapture1(TIM5);
				//CC1P=0 设置为上升沿捕获
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);

                /**重新开始下一步捕获 */
                RestartCapture();
			}
			//还未开始,第一次捕获上升沿
			else  								
			{
				//清空
				devicePluseCaptureState=0;			
				devicePluseCaptureVal=0;
				//标记捕获到了上升沿
				devicePluseCaptureState|=0X40;	
				//关闭定时器5
				TIM_Cmd(TIM5,DISABLE ); 	
				TIM_SetCounter(TIM5,0);
				//CC1P=1 设置为下降沿捕获
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);	
				//使能定时器5				
				TIM_Cmd(TIM5,ENABLE ); 	
			}		    
		}			     	    					   
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
}



























