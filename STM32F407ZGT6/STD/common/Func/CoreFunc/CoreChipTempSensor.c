#include "CoreChipTempSensor.h"

/**初始化传感器 */
void CoreChipTempSensorInit(void)
{
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	//使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	
	//复位结束
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);		 

	//使能内部温度传感器
	ADC_TempSensorVrefintCmd(ENABLE);

	//独立模式
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	//DMA失能
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 
	ADC_CommonInit(&ADC_CommonInitStructure);

	//12位模式
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	//非扫描模式	
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//ADC16,ADC通道,480个周期,提高采样时间可以提高精确度	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );	
	//开启AD转换器	
	ADC_Cmd(ADC1, ENABLE);
}

/**获取当前温度值 */
float CoreChipTempSensorGet(uint16_t times)
{
    uint32_t channelValueSum = 0;
    uint16_t timesLocal = times;
    double temperate;
    if(times == 0)
        return 0;
    //设置指定ADC的规则组通道，一个序列，采样时间
	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );
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
    channelValueSum = (uint16_t)(channelValueSum/times); 
    //电压值
	temperate=(float)channelValueSum*(3.3/4096);
	//转换为温度值 
	temperate=(temperate-0.76)/0.0025 + 25;
    return (float)temperate;
}

