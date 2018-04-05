#include "DevicePluseCapture.h"
#include "SystemUtil.h"

//定时器5通道1输入捕获配置
TIM_ICInitTypeDef  TIM5_ICInitStructure;

/**缓存捕获次数和时间 */
static uint32_t devicePluseCaptureCount = 0;

/**缓存捕获次数和时间 ,单位是微秒*/
static uint32_t devicePluseCaptureTime = 0;

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
// 输入捕获状态
uint8_t  devicePluseCaptureState = 0;

/*输入捕获值(TIM5是16位)*/
uint16_t devicePluseCaptureVal;


/*设备初始化*/
void  DevicePluseCaptureInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //使能TIM5时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    //PA0 清除之前设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    //PA0 输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    //PA0 下拉 上升沿触发捕获
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);

    //初始化定时器5 TIM5 设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
    //预分频器
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    //初始化TIM5输入捕获参数,CC1S=01,选择输入端,IC1映射到TI1上
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;
    //上升沿捕获
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    //映射到TI1上
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    //配置输入分频,不分频
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    //IC1F=0000 配置输入滤波器 滤波
    TIM5_ICInitStructure.TIM_ICFilter = 0x0F;
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //中断分组初始化 TIM5中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_PLUSE_CAPTURE_PREE_PRI;
    //从优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_PLUSE_CAPTURE_SUB_PRI;
    //IRQ通道被使能
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    NVIC_Init(&NVIC_InitStructure);

    //允许更新中断 ,允许CC1IE捕获中断
    TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE);
    //使能定时器5
    TIM_Cmd(TIM5, ENABLE);
}

/*获取捕获次数以及最大的捕获时间*/
uint32_t DevicePluseCaptureGetCountWithTime(uint32_t* maxTimer)
{
    uint32_t count = 0;
    *maxTimer = devicePluseCaptureTime;
    count = devicePluseCaptureCount;
    devicePluseCaptureTime = 0;
    devicePluseCaptureCount = 0;
    return count;
}

void TIM5_IRQHandler(void)
{
    /**还未成功捕获	 */
    if((devicePluseCaptureState&0X80)==0)
	{
		if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
		{
            /**已经捕获到高电平了 */
			if(devicePluseCaptureState&0X40)
			{
                /**高电平太长了 */
				if((devicePluseCaptureState&0X3F)==0X3F)
				{
                    /**标记成功捕获了一次 */
					devicePluseCaptureState|=0X80;
					devicePluseCaptureVal=0XFFFF;
				}
                /**记录捕获时间 */
                else
                    devicePluseCaptureState++;
			}
		}
        /**捕获1发生捕获事件 */
	    if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
		{
            /**捕获到一个下降沿 	 */
			if(devicePluseCaptureState&0X40)
			{
                /**标记成功捕获到一次上升沿 */
				devicePluseCaptureState|=0X80;
				devicePluseCaptureVal=TIM_GetCapture1(TIM5);
                /**CC1P=0 设置为上升沿捕获 */
		   		TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);
                /**重新开始下一步捕获 */
                devicePluseCaptureCount++;
                uint32_t temp = 0;
                temp = (devicePluseCaptureVal + ((devicePluseCaptureState & 0X3F) * 65535));
                if (temp > devicePluseCaptureTime)
                {
                    devicePluseCaptureTime = temp;
                }
                /**初始化状态 */
                devicePluseCaptureState = 0;
                devicePluseCaptureVal = 0;
			}
            /**还未开始,第一次捕获上升沿 */
            else
			{
                /**清空 */
				devicePluseCaptureState=0;
				devicePluseCaptureVal=0;
	 			TIM_SetCounter(TIM5,0);
                /**标记捕获到了上升沿 */
				devicePluseCaptureState|=0X40;
                /**CC1P=1 设置为下降沿捕获 */
		   		TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);
			}
		}
 	}
    /**清除中断标志位 */
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update);
}



