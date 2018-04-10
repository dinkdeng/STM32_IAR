#include "DeviceLightSensor.h"

/**光敏传感器初始化*/
//这里我们仅以规则通道为例
void DeviceLightSensorInit(void)
{
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    //使能ADC3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	//ADC3复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);
	//复位结束
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);

	//先初始化ADC3通道7IO口 通道7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	//模拟输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//不带上下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	//初始化
	GPIO_Init(GPIOF, &GPIO_InitStructure);

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
	ADC_Init(ADC3, &ADC_InitStructure);

	//开启AD转换器
	ADC_Cmd(ADC3, ENABLE);
}

/**光敏传感器获取ADC的值*/
uint16_t DeviceLightSensorGetPowerful(uint8_t times)
{
    uint32_t channelValueSum = 0;
    uint16_t timesLocal = times;
    if(times == 0)
        return 0;
    //设置指定ADC的规则组通道，一个序列，采样时间
	//ADC3,ADC通道,480个周期,提高采样时间可以提高精确度
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_480Cycles );
    while(timesLocal > 0)
    {
        //使能指定的ADC3的软件转换启动功能
        ADC_SoftwareStartConv(ADC3);

        //等待转换结束,注意,此处应该做转换的超时处理
        while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));
        //返回最近一次ADC1规则组的转换结果
        channelValueSum += ADC_GetConversionValue(ADC3);
        timesLocal--;
    }
    return (uint16_t)(channelValueSum/times);
}