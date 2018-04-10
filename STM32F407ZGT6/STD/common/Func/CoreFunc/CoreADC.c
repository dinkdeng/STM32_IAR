#include "CoreADC.h"

/**初始化 */
void CoreADC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 

	//先初始化ADC1通道5 IO口 PA5 通道5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	//模拟输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//不带上下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	 
	//复位结束
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);		 

	//独立模式
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	//DMA失能
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 	
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	//初始化
	ADC_CommonInit(&ADC_CommonInitStructure);

	//12位模式
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	//非扫描模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	//关闭连续转换
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//右对齐	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	//ADC初始化
	ADC_Init(ADC1, &ADC_InitStructure);

	//开启AD转换器
	ADC_Cmd(ADC1, ENABLE);
}

/**获取指定通道的ADC的值 */
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
uint16_t CoreADC_GetValueBychannel(uint32_t channel,uint16_t times)
{
    uint32_t channelValueSum = 0;
    uint16_t timesLocal = times;
    if(times == 0)
        return 0;
    //设置指定ADC的规则组通道，一个序列，采样时间
	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度	
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_480Cycles );
    while(timesLocal > 0)
    {
        //使能指定的ADC1的软件转换启动功能
        ADC_SoftwareStartConv(ADC1);			
        //等待转换结束
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        //返回最近一次ADC1规则组的转换结果
        channelValueSum += ADC_GetConversionValue(ADC1);
        timesLocal--;
    }
    return (uint16_t)(channelValueSum/times);    
}



