#include "CoreDAC.h"

/**DAC初始化 */
void CoreDACInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    //使能PORTA通道时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //使能DAC通道时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // 端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    //模拟输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //PA.4 输出高
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    //不使用触发功能 TEN1=0
    DAC_InitType.DAC_Trigger = DAC_Trigger_None;
    //不使用波形发生
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    //屏蔽、幅值设置
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    //DAC1输出缓存关闭 BOFF1=1
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    //初始化DAC通道1
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    //使能DAC1
    DAC_Cmd(DAC_Channel_1, ENABLE);
    //12位右对齐数据格式设置DAC值
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

/**DAC设置参数值 */
void CoreDACSetValue(uint32_t channel, uint16_t vol)
{
    if (channel == DAC_Channel_1)
    {
        //12位右对齐数据格式设置DAC值
        DAC_SetChannel1Data(DAC_Align_12b_R, vol);
    }
    else
    {
        //12位右对齐数据格式设置DAC值
        DAC_SetChannel2Data(DAC_Align_12b_R, vol);
    }
}


