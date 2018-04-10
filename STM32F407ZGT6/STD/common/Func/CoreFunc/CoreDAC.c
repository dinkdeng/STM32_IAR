#include "CoreDAC.h"

/**DAC初始化 */
void CoreDAC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//使能DAC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	//模拟输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//下拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	//初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//不使用触发功能 TEN1=0
	DAC_InitType.DAC_Trigger=DAC_Trigger_None;
	//不使用波形发生
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;
	//屏蔽、幅值设置
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
	//DAC1输出缓存关闭 BOFF1=1
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;
	//初始化DAC通道1
	DAC_Init(DAC_Channel_1,&DAC_InitType);

	//使能DAC通道1
	DAC_Cmd(DAC_Channel_1, ENABLE);

	//12位右对齐数据格式设置DAC值
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

/**DAC设置参数值 */
//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void CoreDAC_SetValue(uint32_t channel, uint16_t vol)
{
    double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	//12位右对齐数据格式设置DAC值
	DAC_SetChannel1Data(DAC_Align_12b_R,(uint16_t)temp);
}